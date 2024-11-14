#ifndef __AVS_MANAGER_H
#define __AVS_MANAGER_H
#include <map>
#include <string>
#include <vector>
#include <memory>
#include "lib/avaspecx64.h"

class AVSManager final
{
   public:
    explicit AVSManager(int port = 0);
    ~AVSManager();
    AVSManager(AVSManager &) = delete;
    AVSManager(AVSManager &&) = delete;
    std::vector<double> measureData(int numberID);
    int findDevice();
    int activateDevice(int numberID);
    int measurePerpare(int numberID, double intergralTime=5, int averagesNum=1);
   private:
    std::string versionInfo_;
    std::unique_ptr<AvsIdentityType[]> avsIdentityList_;
    unsigned short numPixelsOfDevice_;
    double *lambdaArrayOfDevice_ = nullptr;
    int activatedDeviceID_ = -1;
    std::map<int, AvsHandle> activatedDeviceListMap_;
};

#endif  //!__AVS_MANAGER_H
