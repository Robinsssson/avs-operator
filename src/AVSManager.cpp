#include "AVSManager.h"

#include <spdlog/spdlog.h>

#include <string>

#include "lib/avaspecx64.h"

AVSManager::AVSManager(int port)
{
    AVS_Init(port);  // use usb port
    char tmp[20];
    AVS_GetDLLVersion(tmp);
    versionInfo_ = std::string(tmp);
    spdlog::info("version of dll version is {}", versionInfo_);
}

int AVSManager::findDevice()
{
    unsigned int numberOfDevices = AVS_UpdateUSBDevices();
    if (!numberOfDevices)
    {
        spdlog::warn("there is not devices is linked!");
        return -1;
    }
    this->avsIdentityList_ = new AvsIdentityType[numberOfDevices];
    auto ret = AVS_GetList(
        numberOfDevices * sizeof(AvsIdentityType), &numberOfDevices, this->avsIdentityList_);
    if (ret == ERR_INVALID_SIZE)
    {
        spdlog::error("ret code happened in {} LINE {}, RET {}", __FUNCTION__, __LINE__, ret);
        return -1;
    }
    return ret;
}

AVSManager::~AVSManager() { AVS_Done(); }
