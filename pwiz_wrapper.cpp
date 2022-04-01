#include "pwiz_wrapper.h"
#include "libs/pwiz/pwiz/data/msdata/MSDataFile.hpp"
#include <cfloat>
#include <numeric>

Header *getAllScanHeaderInfo (MSDataFile*file ) {
    auto msd = ((pwiz::msdata::MSDataFile *) file->msd);
    if (msd != nullptr) {
        if (file->header == nullptr) {
            auto slp = msd->run.spectrumListPtr;
            size_t N = slp->size();
            auto allscans = std::vector<size_t>(N);
            std::iota (std::begin(allscans), std::end(allscans), 0);
            file->header = getScanHeader(file,&allscans);
        }
        return (Header*)file->header ;
    }
    return {};
}

Header *getScanHeader(MSDataFile *file, std::vector<size_t> *whichScans) {
    if(file->header == nullptr) {
        auto msd = ((pwiz::msdata::MSDataFile *) file->msd);
        auto header = new Header;
        auto slp = msd->run.spectrumListPtr;
        auto specCount = slp->size();
        auto scanCount = whichScans->size();
        std::vector<size_t> seqNum(scanCount, SIZE_MAX);
        std::vector<size_t> acquisitionNum(scanCount, SIZE_MAX);
        std::vector<int> msLevel(scanCount, INT_MIN);
        std::vector<int> polarity(scanCount, INT_MIN);
        std::vector<int> peaksCount(scanCount, INT_MIN);
        std::vector<double> totIonCurrent(scanCount, DBL_MIN);
        std::vector<double> retentionTime(scanCount, DBL_MIN);        /* in seconds */
        std::vector<double> basePeakMZ(scanCount, DBL_MIN);
        std::vector<double> basePeakIntensity(scanCount, DBL_MIN);
        std::vector<double> collisionEnergy(scanCount, DBL_MIN);
        std::vector<double> ionisationEnergy(scanCount, DBL_MIN);
        std::vector<double> lowMZ(scanCount, DBL_MIN);
        std::vector<double> highMZ(scanCount, DBL_MIN);
        std::vector<size_t> precursorScanNum(scanCount, SIZE_MAX); /* only if MS level > 1 */
        std::vector<double> precursorMZ(scanCount, DBL_MIN);  /* only if MS level > 1 */
        std::vector<int> precursorCharge(scanCount, INT_MIN);  /* only if MS level > 1 */
        std::vector<double> precursorIntensity(scanCount, DBL_MIN);  /* only if MS level > 1 */
        std::vector<size_t> mergedScan(scanCount, SIZE_MAX);  /* only if MS level > 1 */
        std::vector<size_t> mergedResultScanNum(scanCount, SIZE_MAX); /* scan number of the resultant merged scan */
        std::vector<size_t> mergedResultStartScanNum(scanCount,
                                                     SIZE_MAX); /* smallest scan number of the scanOrigin for merged scan */
        std::vector<size_t> mergedResultEndScanNum(scanCount,
                                                   SIZE_MAX); /* largest scan number of the scanOrigin for merged scan */
        std::vector<double> ionInjectionTime(scanCount, DBL_MIN); /* The time spent filling an ion trapping device*/
        std::vector<std::string> filterString(scanCount, "");
        std::vector<std::string> spectrumId(scanCount, "");
        std::vector<bool> centroided(scanCount, false);
        std::vector<double> ionMobilityDriftTime(scanCount, DBL_MIN);
        std::vector<double> isolationWindowTargetMZ(scanCount, DBL_MIN);
        std::vector<double> isolationWindowLowerOffset(scanCount, DBL_MIN);
        std::vector<double> isolationWindowUpperOffset(scanCount, DBL_MIN);
        std::vector<double> scanWindowLowerLimit(scanCount, DBL_MIN);
        std::vector<double> scanWindowUpperLimit(scanCount, DBL_MIN);

        for (size_t i = 0; i < scanCount; i++) {
            auto scanNum = (*whichScans)[i];
            auto index = scanNum - 1;
            auto spectrum = slp->spectrum(index, pwiz::msdata::DetailLevel_FullMetadata);
            pwiz::msdata::Scan dummy;
            auto &scan = spectrum->scanList.scans.empty() ? dummy : spectrum->scanList.scans[0];
            auto polarParam = spectrum->cvParamChild(pwiz::cv::MS_scan_polarity);
            auto centr = spectrum->cvParamChild(pwiz::cv::MS_spectrum_representation);

            seqNum[i] = scanNum;
            acquisitionNum[i] = getAcquisitionNumber(file, spectrum->id, index);
            msLevel[i] = spectrum->cvParam(pwiz::cv::MS_ms_level).valueAs<int>();
            polarity[i] = (polarParam.cvid == pwiz::cv::MS_negative_scan ? 0 : (polarParam.cvid ==
                                                                                pwiz::cv::MS_positive_scan ? +1 : -1));
            peaksCount[i] = static_cast<int>(spectrum->defaultArrayLength);
            totIonCurrent[i] = spectrum->cvParam(pwiz::cv::MS_total_ion_current).valueAs<double>();
            retentionTime[i] = scan.cvParam(pwiz::cv::MS_scan_start_time).timeInSeconds();
            basePeakMZ[i] = spectrum->cvParam(pwiz::cv::MS_base_peak_m_z).valueAs<double>();
            basePeakIntensity[i] = spectrum->cvParam(pwiz::cv::MS_base_peak_intensity).valueAs<double>();
            ionisationEnergy[i] = spectrum->cvParam(pwiz::cv::MS_ionization_energy_OBSOLETE).valueAs<double>();
            lowMZ[i] = spectrum->cvParam(pwiz::cv::MS_lowest_observed_m_z).valueAs<double>();
            highMZ[i] = spectrum->cvParam(pwiz::cv::MS_highest_observed_m_z).valueAs<double>();
            centroided[i] = centr.cvid == pwiz::cv::MS_centroid_spectrum;
            retentionTime[i] = scan.cvParam(pwiz::cv::MS_scan_start_time).timeInSeconds();
            ionInjectionTime[i] = (scan.cvParam(pwiz::cv::MS_ion_injection_time).timeInSeconds() * 1000);
            filterString[i] = scan.cvParam(pwiz::cv::MS_filter_string).value.empty() ? "" : scan.cvParam(
                    pwiz::cv::MS_filter_string).value;
            ionMobilityDriftTime[i] = scan.cvParam(pwiz::cv::MS_ion_mobility_drift_time).value.empty() ? DBL_MIN : (
                    scan.cvParam(pwiz::cv::MS_ion_mobility_drift_time).timeInSeconds() * 1000);
            if (!scan.scanWindows.empty()) {
                scanWindowLowerLimit[i] = scan.scanWindows[0].cvParam(
                        pwiz::cv::MS_scan_window_lower_limit).valueAs<double>();
                scanWindowUpperLimit[i] = scan.scanWindows[0].cvParam(
                        pwiz::cv::MS_scan_window_upper_limit).valueAs<double>();
            }
            if (!spectrum->precursors.empty()) {
                const auto &precursor = spectrum->precursors[0];
                // collisionEnergy
                collisionEnergy[i] = precursor.activation.cvParam(pwiz::cv::MS_collision_energy).valueAs<double>();
                // precursorScanNum
                size_t precursorIndex = slp->find(precursor.spectrumID);
                if (precursorIndex < specCount) {
                    precursorScanNum[i] = getAcquisitionNumber(file, precursor.spectrumID, precursorIndex);
                }
                // precursorMZ, precursorCharge, precursorIntensity
                if (!precursor.selectedIons.empty()) {
                    precursorMZ[i] = precursor.selectedIons[0].cvParam(pwiz::cv::MS_selected_ion_m_z).value.empty()
                                     ? precursor.selectedIons[0].cvParam(pwiz::cv::MS_m_z).valueAs<double>()
                                     : precursor.selectedIons[0].cvParam(
                                    pwiz::cv::MS_selected_ion_m_z).valueAs<double>();
                    precursorCharge[i] = precursor.selectedIons[0].cvParam(pwiz::cv::MS_charge_state).valueAs<int>();
                    precursorIntensity[i] = precursor.selectedIons[0].cvParam(
                            pwiz::cv::MS_peak_intensity).valueAs<double>();
                }
                // isolationWindowTargetMZ, ...
                auto iwin = spectrum->precursors[0].isolationWindow;
                if (!iwin.empty()) {
                    isolationWindowTargetMZ[i] = iwin.cvParam(pwiz::cv::MS_isolation_window_target_m_z).value.empty()
                                                 ? DBL_MIN : iwin.cvParam(
                                    pwiz::cv::MS_isolation_window_target_m_z).valueAs<double>();
                    isolationWindowLowerOffset[i] = iwin.cvParam(
                            pwiz::cv::MS_isolation_window_lower_offset).value.empty()
                                                    ? DBL_MIN : iwin.cvParam(
                                    pwiz::cv::MS_isolation_window_lower_offset).valueAs<double>();
                    isolationWindowUpperOffset[i] = iwin.cvParam(
                            pwiz::cv::MS_isolation_window_upper_offset).value.empty()
                                                    ? DBL_MIN : iwin.cvParam(
                                    pwiz::cv::MS_isolation_window_upper_offset).valueAs<double>();
                }
            }
        }

        header->insert({"seqNum", std::make_any<std::vector<size_t>>(seqNum)});
        header->insert({"acquisitionNum", std::make_any<std::vector<size_t>>(acquisitionNum)});
        header->insert({"msLevel", std::make_any<std::vector<int>>(msLevel)});
        header->insert({"polarity", std::make_any<std::vector<int>>(polarity)});
        header->insert({"peaksCount", std::make_any<std::vector<int>>(peaksCount)});
        header->insert({"totIonCurrent", std::make_any<std::vector<double>>(totIonCurrent)});
        header->insert({"retentionTime", std::make_any<std::vector<double>>(retentionTime)});
        header->insert({"basePeakMZ", std::make_any<std::vector<double>>(basePeakMZ)});
        header->insert({"basePeakIntensity", std::make_any<std::vector<double>>(basePeakIntensity)});
        header->insert({"collisionEnergy", std::make_any<std::vector<double>>(collisionEnergy)});
        header->insert({"ionisationEnergy", std::make_any<std::vector<double>>(ionisationEnergy)});
        header->insert({"lowMZ", std::make_any<std::vector<double>>(lowMZ)});
        header->insert({"highMZ", std::make_any<std::vector<double>>(highMZ)});
        header->insert({"precursorScanNum", std::make_any<std::vector<size_t>>(precursorScanNum)});
        header->insert({"precursorMZ", std::make_any<std::vector<double>>(precursorMZ)});
        header->insert({"precursorCharge", std::make_any<std::vector<int>>(precursorCharge)});
        header->insert({"precursorIntensity", std::make_any<std::vector<double>>(precursorIntensity)});
        header->insert({"mergedScan", std::make_any<std::vector<size_t>>(mergedScan)});
        header->insert({"mergedResultScanNum", std::make_any<std::vector<size_t>>(mergedResultScanNum)});
        header->insert({"mergedResultStartScanNum", std::make_any<std::vector<size_t>>(mergedResultStartScanNum)});
        header->insert({"mergedResultEndScanNum", std::make_any<std::vector<size_t>>(mergedResultEndScanNum)});
        header->insert({"injectionTime", std::make_any<std::vector<double>>(ionInjectionTime)});
        header->insert({"filterString", std::make_any<std::vector<std::string>>(filterString)});
        header->insert({"spectrumId", std::make_any<std::vector<std::string>>(spectrumId)});
        header->insert({"centroided", std::make_any<std::vector<bool>>(centroided)});
        header->insert({"ionMobilityDriftTime", std::make_any<std::vector<double>>(ionMobilityDriftTime)});
        header->insert({"isolationWindowTargetMZ", std::make_any<std::vector<double>>(isolationWindowTargetMZ)});
        header->insert({"isolationWindowLowerOffset", std::make_any<std::vector<double>>(isolationWindowLowerOffset)});
        header->insert({"isolationWindowUpperOffset", std::make_any<std::vector<double>>(isolationWindowUpperOffset)});
        header->insert({"scanWindowLowerLimit", std::make_any<std::vector<double>>(scanWindowLowerLimit)});
        header->insert({"scanWindowUpperLimit", std::make_any<std::vector<double>>(scanWindowUpperLimit)});
        file->header = header;
    }
    return (Header *)file->header;
}

size_t getAcquisitionNumber(MSDataFile *file, const std::string &id, size_t index) {
    std::string scanNumber = pwiz::msdata::id::translateNativeIDToScanNumber(getNativeIdFormat(file), id);
    if (scanNumber.empty())
        return index + 1;
    else
        return std::stoul(scanNumber);
}

pwiz::cv::CVID getNativeIdFormat(MSDataFile *file) {
    auto msd = ((pwiz::msdata::MSDataFile *) file->msd);
    return pwiz::msdata::id::getDefaultNativeIDFormat(*msd);
}


PeakList getPeakList(MSDataFile *file, std::vector<size_t> whichScan) {
    auto msd = ((pwiz::msdata::MSDataFile *) file->msd);
    size_t scanCount = whichScan.size();
    std::vector<std::vector<double>> peakList(scanCount);
    if (msd != nullptr) {
        auto slp = msd->run.spectrumListPtr;
        size_t spectrumCount = slp->size();
        size_t currentScan;
        pwiz::msdata::SpectrumPtr sp;
        pwiz::msdata::BinaryDataArrayPtr mzs, ints;
        std::vector<double> data;
        for (size_t i = 0; i < scanCount; i++) {
            currentScan = whichScan[i];
            if (currentScan < 1 || currentScan > spectrumCount) {
                return {};
            }
            size_t currentIndex = static_cast<size_t>(currentScan - 1);
            sp = slp->spectrum(currentIndex, pwiz::msdata::DetailLevel_FullData);
            mzs = sp->getMZArray();
            ints = sp->getIntensityArray();
            if (!mzs.get() || !ints.get()) {
                peakList[i] = std::vector<double>(2);
                continue;
            }
            if (mzs->data.size() != ints->data.size())
                data = mzs->data;
            data.insert(data.end(), ints->data.begin(), ints->data.end());
            peakList[i] = data;
        }
    }
    return peakList;
}

std::string getRunStartTimeStamp(MSDataFile * file) {
    auto msd = ((pwiz::msdata::MSDataFile *) file->msd);
    if (msd != NULL) {
        return msd->run.startTimeStamp;
    }
    return "";
}