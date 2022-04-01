#ifndef MZ_PWIZ_C_INTERFACE_H
#define MZ_PWIZ_C_INTERFACE_H


#ifdef __cplusplus
#define MAYBE_UNUSED [[maybe_unused]]
#include <cstdint>
extern "C" {
#else
#define MAYBE_UNUSED
#include <stdint.h>
#endif


    typedef struct RunInfo {
        uint64_t scanCount;
        double* lowMz;
        double* highMz;
        double dStartTime;
        double dEndTime;
        int * msLevels;
        const char* startTimeStamp;
    } RunInfo;
typedef struct InstrumentInfo {
    const char *manufacturer;
    const char *model;
    const char *ionisation;
    const char *analyzer;
    const char *detector;
    const char *software;
    const char *sample;
    const char *source;
} InstrumentInfo;

typedef struct MSDataFile {
    void *msd;
    char filename[255];
    InstrumentInfo *instrumentInfo;
    void *header;
    void *runInfo;
} MSDataFile;

/*
pwiz.version()

# Read/Write
copyWriteMSData(object, file, original_file, header, backend ="pwiz", outformat = "mzml", rtime_seconds = TRUE, software_processing)
openMSfile(filename, backend = NULL, verbose = FALSE)
initializeRamp(object)
isInitialized(object)
fileName(object, ...)
openIDfile(filename, verbose = FALSE)
writeMSData(object, file, header, backend = "pwiz", outformat = "mzml", rtime_seconds = TRUE, software_processing)

# Analyze
isolationWindow
    signature(object = "mzRpwiz", unique. = "logical", simplify = "logical")

# Metadata
v runInfo(object)
- chromatogramsInfo(object)
v analyzer(object)
v detector(object)
v instrumentInfo(object)
v ionisation(object)
v softwareInfo(object)
v sampleInfo(object)
v sourceInfo(object)
v model(object)
i mzidInfo(object)
i modifications(object, ...)
i psms(object, ...)
i substitutions(object)
i database(object, ...)
i enzymes(object)
i tolerance(object, ...)
i score(x, ...)
i para(object)
i specParams(object)

# Data Info
v analyzer signature(object = "mzRpwiz"): ...
v detector signature(object = "mzRpwiz"): ...
v instrumentInfo signature(object = "mzRpwiz"): ...
v ionisation signature(object = "mzRpwiz"): ...
length signature(x = "mzRpwiz"): ...
v manufacturer signature(object = "mzRpwiz"): ...
v model signature(object = "mzRpwiz"): ...
v runInfo signature(object = "mzRpwiz"): ...
- chromatogramsInfo signature(object = "mzRpwiz"):

# Peaks
 header(object, scans, ...)
peaksCount(object, scans, ...)
## S4 method for signature 'mzRpwiz'
peaks(object, scans)
## S4 method for signature 'mzRramp'
peaks(object, scans)
## S4 method for signature 'mzRnetCDF'
peaks(object, scans)
## S4 method for signature 'mzRpwiz'
spectra(object, scans) ## same as peaks
## S4 method for signature 'mzRramp'
spectra(object, scans)
## S4 method for signature 'mzRnetCDF'
spectra(object, scans)
get3Dmap(object, scans, lowMz, highMz, resMz, ...)
## S4 method for signature 'mzRpwiz'
chromatogram(object, chrom)
## S4 method for signature 'mzRpwiz'
chromatograms(object, chrom) ## same as chromatogram
## S4 method for signature 'mzRpwiz'
chromatogramHeader(object, chrom)
tic(object, ...)
nChrom(object)
 */

MAYBE_UNUSED MSDataFile *openMSData(char *filename);
MAYBE_UNUSED const char *getFileName(MSDataFile *file);
MAYBE_UNUSED uint64_t getLastScan(const MSDataFile *file);
MAYBE_UNUSED uint64_t getLastChrom(const MSDataFile *file);

// Metadata
MAYBE_UNUSED RunInfo * getRunInfo(MSDataFile *file);
MAYBE_UNUSED const char* getAnalyzer(MSDataFile *file);
MAYBE_UNUSED const char* getDetector(MSDataFile *file);
MAYBE_UNUSED InstrumentInfo *getInstrumentInfo(MSDataFile *file);
MAYBE_UNUSED const char* getIonisation(MSDataFile *file);
MAYBE_UNUSED const char* getSoftwareInfo(MSDataFile *file);
MAYBE_UNUSED const char* getSampleInfo(MSDataFile *file);
MAYBE_UNUSED const char* getSourceInfo(MSDataFile *file);
MAYBE_UNUSED const char* getModel(MSDataFile *file);
MAYBE_UNUSED const char* getManufacturer(MSDataFile *file);
#ifdef __cplusplus
}
#endif
#endif //MZ_PWIZ_C_INTERFACE_H
