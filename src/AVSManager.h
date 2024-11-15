#ifndef __AVS_MANAGER_H
#define __AVS_MANAGER_H
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "lib/avaspecx64.h"

class AVSManager final {
   public:
    enum class AdjustMethod {
        maximum,
        average
    };
    explicit AVSManager(int port = 0);
    ~AVSManager();
    AVSManager(AVSManager &) = delete;
    AVSManager(AVSManager &&) = delete;
    std::tuple<std::vector<double>, std::time_t> measureData(int numberID);
    int findDevice();
    int activateDevice(int numberID);
    int measurePerpare(int numberID, double intergralTime = 5, int averagesNum = 1);
    int saveDataInFile(std::string filePath, std::vector<double> data, time_t time);
    int getActivateID() const { return activatedDeviceID_; }
    int adjustVal(const std::vector<double> &data, double angle, AdjustMethod method=AdjustMethod::average);

   private:
    std::string versionInfo_;
    std::unique_ptr<AvsIdentityType[]> avsIdentityList_;
    unsigned short numPixelsOfDevice_;
    double *lambdaArrayOfDevice_ = nullptr;
    int activatedDeviceID_ = -1;
    std::map<int, AvsHandle> activatedDeviceListMap_;
};

#endif  //!__AVS_MANAGER_H
