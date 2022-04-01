#include "mz_pwiz_c_interface.h"

#include <iostream>
#include "libs/pwiz/pwiz/data/msdata/MSDataFile.hpp"
#include "libs/pwiz/pwiz/data/msdata/LegacyAdapter.hpp"
#include "pwiz/data/common/CVTranslator.hpp"
#include "pwiz_wrapper.h"


[[maybe_unused]] MSDataFile *openMSData(char *filename) {
    auto datafile = new MSDataFile;
    datafile->msd = new pwiz::msdata::MSDataFile(filename);
    strcpy(datafile->filename, filename);
    return datafile;
}

[[maybe_unused]] const char *getFileName(MSDataFile *file) {
    if (file != nullptr) {
        return file->filename;
    }
    return "";
}

uint64_t getLastScan(const MSDataFile *file) {
    if (file != nullptr && file->msd != nullptr) {
        auto slp = ((pwiz::msdata::MSDataFile *) file->msd)->run.spectrumListPtr;
        return slp->size();
    }
    return -1;
}

[[maybe_unused]] uint64_t getLastChrom(const MSDataFile *file) {
    if (file != nullptr && file->msd != nullptr) {
        auto slp = ((pwiz::msdata::MSDataFile *) file->msd)->run.chromatogramListPtr;
        return slp->size();
    }
    return -1;
}

[[maybe_unused]] InstrumentInfo *getInstrumentInfo(MSDataFile *file) {
    if (file != nullptr && file->msd != nullptr && file->instrumentInfo == nullptr) {
        file->instrumentInfo = new InstrumentInfo{};
        auto msd = ((pwiz::msdata::MSDataFile *) file->msd);
        auto icp = msd->instrumentConfigurationPtrs;
        if (!icp.empty()) {
            pwiz::data::CVTranslator cvTranslator;
            pwiz::msdata::LegacyAdapter_Instrument adapter(*icp[0], cvTranslator);
            auto sp = msd->softwarePtrs;
            auto sample = msd->samplePtrs;
            auto scanSetting = msd->scanSettingsPtrs;

            try {
                file->instrumentInfo->ionisation = strdup(adapter.ionisation().c_str());
            } catch (...) {}
            try {
                file->instrumentInfo->analyzer = strdup(adapter.analyzer().c_str());
            } catch (...) {}
            try {
                file->instrumentInfo->detector = strdup(adapter.detector().c_str());
            } catch (...) {}
            file->instrumentInfo = new InstrumentInfo{strdup(adapter.manufacturer().c_str()),
                                                      strdup(adapter.model().c_str()),
                                                      !sp.empty() ? strdup((sp[0]->id + " " + sp[0]->version).c_str())
                                                                  : "", !sample.empty() ? strdup(
                            (sample[0]->name + sample[0]->id).c_str()) : "", !scanSetting.empty()
                                                                             ? (scanSetting[0]->sourceFilePtrs[0]->location).c_str()
                                                                             : ""};
        }
    }
    return file->instrumentInfo;
}

const char *getModel(MSDataFile *file) {
    return getInstrumentInfo(file)->model;
}

[[maybe_unused]] RunInfo *getRunInfo(MSDataFile *file) {
    if (file->runInfo == nullptr) {
        auto header = getAllScanHeaderInfo(file);
        auto info = new RunInfo{};
        info->lowMz = std::any_cast<std::vector<double>>(header->at("lowMz")).data();
        info->highMz = std::any_cast<std::vector<double>>(header->at("highMz")).data();
        info->dStartTime = *std::min_element(std::any_cast<std::vector<double>>(header->at("retentionTime")).begin(),
                                             std::any_cast<std::vector<double>>(header->at("retentionTime")).end());
        info->dEndTime = *std::max_element(std::any_cast<std::vector<double>>(header->at("retentionTime")).begin(),
                                           std::any_cast<std::vector<double>>(header->at("retentionTime")).end());
        std::vector<int> msLevels{};
        std::unique_copy(std::any_cast<std::vector<int>>(header->at("msLevel")).begin(),
                         std::any_cast<std::vector<int>>(header->at("msLevel")).end(), msLevels.begin());
        info->msLevels = msLevels.data();
        info->startTimeStamp = getRunStartTimeStamp(file).c_str();
        file->runInfo = info;
    }
    return (RunInfo *) file->runInfo;
}

[[maybe_unused]] const char *getAnalyzer(MSDataFile *file) {
    return getInstrumentInfo(file)->analyzer;
}

[[maybe_unused]] const char *getDetector(MSDataFile *file) {
    return getInstrumentInfo(file)->detector;
}

const char *getIonisation(MSDataFile *file) {
    return getInstrumentInfo(file)->ionisation;
}

[[maybe_unused]] const char *getSoftwareInfo(MSDataFile *file) {
    return getInstrumentInfo(file)->software;
}

[[maybe_unused]] const char *getSampleInfo(MSDataFile *file) {
    return getInstrumentInfo(file)->sample;
}

[[maybe_unused]] const char *getSourceInfo(MSDataFile *file) {
    return getInstrumentInfo(file)->source;
}

[[maybe_unused]] const char *getManufacturer(MSDataFile *file) {
    return getInstrumentInfo(file)->manufacturer;
}

