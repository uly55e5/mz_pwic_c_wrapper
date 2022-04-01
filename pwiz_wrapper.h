//
// Created by david on 31.03.22.
//

#ifndef PWIZMZLIB_PWIZ_WRAPPER_H
#define PWIZMZLIB_PWIZ_WRAPPER_H
#include <map>
#include <vector>
#include <any>
#include "mz_pwiz_c_interface.h"
#include "libs/pwiz/pwiz/data/msdata/MSDataFile.hpp"

typedef std::map<std::string,std::any> Header;
typedef std::vector<std::vector<double>> PeakList;
Header *getScanHeader(MSDataFile *file, std::vector<size_t> *whichScans);
size_t getAcquisitionNumber(MSDataFile*file,const std::string& id, size_t index);
pwiz::cv::CVID getNativeIdFormat(MSDataFile*file);
PeakList getPeakList(MSDataFile *file, std::vector<size_t> whichScan);
std::string getRunStartTimeStamp(MSDataFile * file);
Header *getAllScanHeaderInfo (MSDataFile*file);




#endif //PWIZMZLIB_PWIZ_WRAPPER_H
