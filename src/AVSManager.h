#ifndef __AVS_MANAGER_H
#define __AVS_MANAGER_H
#include <vector>
#include <string>
#include <iostream>
#include "lib/avaspecx64.h"
class AVSManager final {
public:
    explicit AVSManager(int port=0);
    ~AVSManager();
    AVSManager(AVSManager&)=delete;
    AVSManager(AVSManager&&)=delete;
    std::vector<double> getValOfLightAnsys();
    int findDevice();
    int activateDevice(int number);
private:
    std::string versionInfo_;
    AvsIdentityType *avsIdentityList_;
};





#endif //!__AVS_MANAGER_H 

