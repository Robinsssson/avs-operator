#include "AVSManager.h"

#include <spdlog/spdlog.h>

#include <exception>
#include <memory>
#include <string>
#include <vector>

#include "Utility.h"
#include "lib/avaspecx64.h"

AVSManager::AVSManager(int port) {
    AVS_Init(port);  // use usb port
    char tmp[20];
    AVS_GetDLLVersion(tmp);
    versionInfo_ = std::string(tmp);
    spdlog::info("AVS DLL version: {}", versionInfo_);
}

int AVSManager::findDevice() {
    unsigned int numberOfDevices = AVS_UpdateUSBDevices();
    if (!numberOfDevices) {
        spdlog::warn("there is not devices is linked!");
        return -1;
    }
    this->avsIdentityList_ = std::make_unique<AvsIdentityType[]>(numberOfDevices);
    auto errorCode = AVS_GetList(numberOfDevices * sizeof(AvsIdentityType), &numberOfDevices, this->avsIdentityList_.get());
    if (LOG_ERROR(errorCode)) return errorCode;
    spdlog::info("Device of Number is {}", errorCode);
    return errorCode;
}

AVSManager::~AVSManager() {
    AVS_Done();
    delete[] this->lambdaArrayOfDevice_;
}

int AVSManager::activateDevice(int number) {
    // Deactivate of last device.
    if (this->activatedDeviceListMap_.find(number) != this->activatedDeviceListMap_.end()) {
        AVS_Deactivate(this->activatedDeviceListMap_[number]);
    }
    // activate the NUMBER device
    auto deviceHandle = AVS_Activate(&this->avsIdentityList_[number]);
    this->activatedDeviceListMap_.insert({number, deviceHandle});
    auto errorCode = AVS_UseHighResAdc(deviceHandle, AVS_ENABLE);
    if (LOG_ERROR(errorCode)) return errorCode;
    spdlog::info("Device {} activated, the Handle is {}", number, deviceHandle);
    errorCode = AVS_GetNumPixels(deviceHandle, &this->numPixelsOfDevice_);
    if (LOG_ERROR(errorCode)) return errorCode;
    this->activatedDeviceID_ = number;
    spdlog::info("Pixels of Device {} is {}", number, this->numPixelsOfDevice_);
    if (this->lambdaArrayOfDevice_ != nullptr) delete[] this->lambdaArrayOfDevice_;
    this->lambdaArrayOfDevice_ = new double[this->numPixelsOfDevice_];
    AVS_GetLambda(deviceHandle, this->lambdaArrayOfDevice_);  // Get \lambda list.
    return 0;
}
void measureHookFunction(AvsHandle *handle, int *val) { spdlog::debug("the handle of measure device is {}, the val is {}", *handle, *val); }
int AVSManager::measurePerpare(int numberID, double intergralTime, int averagesNum) {
    MeasConfigType measConfigure;
    measConfigure.m_StartPixel = 0;
    measConfigure.m_StopPixel = this->numPixelsOfDevice_ - 1;
    measConfigure.m_IntegrationTime = intergralTime;  // 积分时间
    measConfigure.m_IntegrationDelay = 0;
    measConfigure.m_NrAverages = averagesNum;  // 单次测量的平均值个数
    measConfigure.m_CorDynDark.m_Enable = 0;
    measConfigure.m_CorDynDark.m_ForgetPercentage = 0;
    measConfigure.m_Smoothing.m_SmoothPix = 0;
    measConfigure.m_Smoothing.m_SmoothModel = 0;
    measConfigure.m_SaturationDetection = 0;
    measConfigure.m_Trigger.m_Mode = 0;
    measConfigure.m_Trigger.m_Source = 0;
    measConfigure.m_Trigger.m_SourceType = 0;
    measConfigure.m_Control.m_StrobeControl = 0;
    measConfigure.m_Control.m_LaserDelay = 0;
    measConfigure.m_Control.m_LaserWidth = 0;
    measConfigure.m_Control.m_LaserWaveLength = 0;
    measConfigure.m_Control.m_StoreToRam = 0;
    int errorCode;
    try {
        errorCode = AVS_PrepareMeasure(this->activatedDeviceListMap_.at(numberID), &measConfigure);
    } catch (const std::out_of_range &e) {
        spdlog::error("mssage: DEVICE {} is not ACTIVATE occupid. FUNCTION {}, LINE {}", numberID, __FUNCTION__, __LINE__);
        return -1;
    }
    errorCode = AVS_MeasureCallback(this->activatedDeviceListMap_.at(numberID), measureHookFunction, 1);
    if (LOG_ERROR(errorCode)) return errorCode;
    spdlog::info("Perpare of Meassage has done");
    return 0;
}
#define AVS_POLLSCAN_NO_DATA_AVAILABLE 0
std::vector<double> AVSManager::measureData(int numberID) {
    int errorCode;
    try {
        do {
            errorCode = AVS_PollScan(this->activatedDeviceListMap_.at(numberID));
        } while (errorCode == AVS_POLLSCAN_NO_DATA_AVAILABLE);
        if (LOG_ERROR(errorCode)) return {};

        unsigned int timeLabel;
        std::vector<double> arrayOfSpectrum(this->numPixelsOfDevice_);

        // 获取数据并存储到 vector 中
        errorCode = AVS_GetScopeData(this->activatedDeviceListMap_.at(numberID), &timeLabel, arrayOfSpectrum.data());

        if (LOG_ERROR(errorCode)) return {};

        return arrayOfSpectrum;
    } catch (const std::out_of_range &e) {
        spdlog::error("DEVICE {} not activate. FUNCTION {}, LINE {}", numberID, __FUNCTION__, __LINE__);
        return {};
    }
}
