#include "AVSManager.h"

#include <serial/serial.h>
#include <spdlog/spdlog.h>
#include <time.h>
#include <winnls.h>

#include <algorithm>
#include <cmath>
#include <ctime>
#include <exception>
#include <filesystem>
#include <fstream>
#include <memory>
#include <numeric>
#include <string>
#include <tuple>
#include <vector>

#include "Utility.h"
#include "fmt/chrono.h"
#include "lib/avaspecx64.h"

static MeasConfigType initMeasConfig(int numPixels, double integralTime, int averagesNum) {
    MeasConfigType measConfigure;
    measConfigure.m_StartPixel = 0;
    measConfigure.m_StopPixel = numPixels - 1;
    measConfigure.m_IntegrationTime = integralTime;  // 积分时间
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
    return measConfigure;
}

AVSManager::AVSManager(int port, int waveBegin, int waveEnding, std::string siteName)
    : waveBegin_(waveBegin), waveEnding_(waveEnding), siteName_(siteName) {
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
    auto errorCode =
        AVS_GetList(numberOfDevices * sizeof(AvsIdentityType), &numberOfDevices, this->avsIdentityList_.get());
    if (HANDLE_ERROR(errorCode)) return errorCode;
    spdlog::info("Device of Number is {}", errorCode);
    return errorCode;
}

AVSManager::~AVSManager() {
    AVS_Done();
    if (this->lambdaArrayOfDevice_ != nullptr) delete[] this->lambdaArrayOfDevice_;
}

int AVSManager::activateDevice(int number) {
    this->activatedDeviceID_ = number;
    // Deactivate of last device.
    if (this->activatedDeviceListMap_.find(number) != this->activatedDeviceListMap_.end()) {
        AVS_Deactivate(this->activatedDeviceListMap_[number]);
    }
    // activate the NUMBER device
    auto deviceHandle = AVS_Activate(&this->avsIdentityList_[number]);
    this->activatedDeviceListMap_.insert({number, deviceHandle});
    auto errorCode = AVS_UseHighResAdc(deviceHandle, AVS_ENABLE);
    if (HANDLE_ERROR(errorCode)) return errorCode;

    spdlog::info("Device {} activated, the Handle is {}", number, deviceHandle);

    errorCode = AVS_GetNumPixels(deviceHandle, &this->numPixelsOfDevice_);
    if (HANDLE_ERROR(errorCode)) return errorCode;

    spdlog::info("Pixels of Device {} is {}", number, this->numPixelsOfDevice_);

    if (this->lambdaArrayOfDevice_ != nullptr) delete[] this->lambdaArrayOfDevice_;
    this->lambdaArrayOfDevice_ = new double[this->numPixelsOfDevice_];
    AVS_GetLambda(deviceHandle, this->lambdaArrayOfDevice_);  // Get \lambda list.
    return 0;
}
void measureHookFunction(AvsHandle *handle, int *val) {
    spdlog::debug("the handle of measure device is {}, the val is {}", *handle, *val);
}

time_t AVSManager::measurePerpare(int numberID, double integralTime, int averagesNum) {
    this->integralTime_ = integralTime;
    this->averagesNum_ = averagesNum;

    MeasConfigType measConfigure = initMeasConfig(this->numPixelsOfDevice_, integralTime, averagesNum);

    int errorCode;
    try {
        errorCode = AVS_PrepareMeasure(this->activatedDeviceListMap_.at(numberID), &measConfigure);
        if (HANDLE_ERROR(errorCode)) return errorCode;
    } catch (const std::out_of_range &e) {
        spdlog::error(
            "mssage: DEVICE {} is not ACTIVATE occupid. FUNCTION {}, LINE {}", numberID, __FUNCTION__, __LINE__);
        return avs_util::getCurrentTimeT();
    }

    errorCode = AVS_MeasureCallback(this->activatedDeviceListMap_.at(numberID), measureHookFunction, 1);
    auto retTime = avs_util::getCurrentTimeT();
    if (HANDLE_ERROR(errorCode)) return errorCode;

    spdlog::info("Perpare of Meassage has done");
    return retTime;
}

std::tuple<std::vector<double>, std::time_t> AVSManager::measureData(int numberID) {
    int errorCode;
    try {
        do {
            errorCode = AVS_PollScan(this->activatedDeviceListMap_.at(numberID));
        } while (errorCode == AVS_POLLSCAN_NO_DATA_AVAILABLE);
        if (HANDLE_ERROR(errorCode)) return {};
        unsigned int timeLabel;
        std::vector<double> arrayOfSpectrum(this->numPixelsOfDevice_);
        // 获取数据并存储到 vector 中
        errorCode = AVS_StopMeasure(this->activatedDeviceListMap_.at(numberID));
        if (HANDLE_ERROR(errorCode)) return {};
        errorCode = AVS_GetScopeData(this->activatedDeviceListMap_.at(numberID), &timeLabel, arrayOfSpectrum.data());
        if (HANDLE_ERROR(errorCode)) return {};
        return std::tuple(arrayOfSpectrum, avs_util::getCurrentTimeT());
    } catch (const std::out_of_range &e) {
        spdlog::error("DEVICE {} not activate. FUNCTION {}, LINE {}", numberID, __FUNCTION__, __LINE__);
        return {};
    }
}

int AVSManager::saveDataInFile(const std::filesystem::path &filePath, std::vector<double> data, time_t inputTimeT,
                               time_t outputTimeT, std::string portCom) {
    // 获取经纬度信息
    bool gpsReaderNormalState = true;
    if (this->getLonAndLat(portCom) != 0) gpsReaderNormalState = false;
    // 构造文件名，确保格式化中不包含非法字符
    auto inputTime = fmt::localtime(inputTimeT);
    std::filesystem::path saveFilePath =
    gpsReaderNormalState ? filePath / fmt::format("{0}{1:03d}{2:%H%M%S}.std", this->angle_ == 90 ? 'L' : 'S', this->angle_, inputTime)
    : filePath / fmt::format("{0}{1:03d}{2:%H%M%S}-gps-error.std", this->angle_ == 90 ? 'L' : 'S', this->angle_, inputTime);
    const auto saveFileName = saveFilePath.filename().string();

    spdlog::info("{} file has been created", saveFileName);
    std::fstream fileStream(saveFilePath, std::ios_base::trunc | std::ios_base::out);

    // 检查文件是否成功打开
    if (!fileStream.is_open()) {
        spdlog::error("Error opening file in function {}, line {}", __FUNCTION__, __LINE__);
        return -1;
    }

    // 写入文件内容
    fileStream << "zenith DOAS\n"
               << "1\n"
               << this->numPixelsOfDevice_ << '\n';

    if (!fileStream) {
        spdlog::error("Error writing header to file {}", saveFileName);
        return -1;
    }

    // 写入数据
    for (const auto &v : data) {
        fileStream << fmt::format("{:.4f}\n", v);
        if (!fileStream) {
            spdlog::error("Error writing data to file {}", saveFileName);
            return -1;
        }
    }

    // 写入其他信息
    fileStream << saveFileName << '\n'
               << "OceanOptics\n"
               << "HR2000+\n"
               << fmt::format("{:%d.%m.%Y}\n", inputTime) << fmt::format("{:%H:%M:%S}\n", inputTime)
               << fmt::format("{:%H:%M:%S}\n", fmt::localtime(outputTimeT))
               << fmt::format("{}\n{}\n", this->waveBegin_, this->waveEnding_)
               << fmt::format("SCANS {}\n", this->averagesNum_) << fmt::format("INT_TIME {}\n", this->integralTime_)
               << fmt::format("SITE {}\n", this->siteName_) << fmt::format("LONGITUDE {}\n", longitude_)
               << fmt::format("LATITUDE {}\n", latitude_);

    if (!fileStream) {
        spdlog::error("Error finalizing file {}", saveFileName);
        return -1;
    }

    // 关闭文件
    fileStream.close();
    return 0;
}

int AVSManager::adjustVal(const std::vector<double> &data, double angle, AVSManager::AdjustMethod method) {
    double adjustValue;
    double summ = std::accumulate(data.begin(), data.end(), 0.0);
    double maxi = *std::max_element(data.begin(), data.end());
    this->angle_ = angle;
    this->method_ = method;
    switch (method) {
        case AdjustMethod::average:
            if (avs_util::isApproximatelyEqual(angle, 90)) {
                adjustValue = 3620400.61632 * std::exp(-(summ / 2048) / 91.29643) +
                              86.36638 * std::exp(-(summ / 2048) / 753.67786) +
                              1033.08165 * std::exp(-(summ / 2048) / 753.53241) + 39.21313;
            } else {
                adjustValue = 62854.56078 * std::exp(-(summ / this->numPixelsOfDevice_) / 142.29071) +
                              6781470000.0 * std::exp(-(summ / this->numPixelsOfDevice_) / 40.54059) +
                              824.25339 * std::exp(-(summ / this->numPixelsOfDevice_) / 636.74826) + 39.99177;
            }
            break;
        case AdjustMethod::maximum:
            adjustValue = 1759198.71151 * std::exp(-maxi / 116.34418) + 3681.8905 * std::exp(-maxi / 445.74025) +
                          218.68263 * std::exp(-maxi / 2556.71114) + 14.01163;
            if (avs_util::isApproximatelyEqual(angle, 90)) {
                adjustValue = 0.6534 * adjustValue - 1.45531;
            }
            break;
    }
    return int(adjustValue);
}

static std::vector<std::string> split(const std::string &str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

static bool extractGNRMCLatLong(const std::string &gnrmc, std::string &latitude, std::string &longitude) {
    // 检查是否以 $GNRMC 开头
    if (gnrmc.find("$GNRMC") != 0) {
        return false;
    }
    //    $GNRMC,044400.00,A,3358.97791,N,11648.40955,E,0.113,,100125,,,A,V*1F
    //    $GNRMC,073002.00,A,3358.98868,N,11648.39866,E,0.032,,120425,,,A,V*11
    // 分割字符串
    std::vector<std::string> tokens = split(gnrmc, ',');
    if (tokens.size() < 6) {
        return false;  // 数据格式不正确
    }

    // 提取纬度和经度
    latitude = tokens[3];   // 纬度
    longitude = tokens[5];  // 经度

    return true;
}

/**
 * @brief 将度分格式转换为十进制度格式
 * @param degreesMinutes 度分格式字符串（例如 "12345.67" 表示 123度45.67分）
 * @return 十进制度字符串，小数部分最多保留8位
 */
static std::string convertDegreesMinutesToDecimal(const std::string &degreesMinutes) {
    double totalValue = std::stod(degreesMinutes);
    int degreesPart = std::round(totalValue);

    // 分离度和分部分
    degreesPart = degreesPart / 100;  // 提取度数部分
    double minutesPart = totalValue - degreesPart * 100;

    // 转换分到十进制小数
    double decimalDegrees = minutesPart / 60 + degreesPart;

    // 格式化输出
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(8) << decimalDegrees;
    std::string result = oss.str();

    // 清除无效尾零和小数点
    size_t decimalPoint = result.find('.');
    if (decimalPoint != std::string::npos) {
        // 从末尾移除多余的零
        size_t lastSignificant = result.find_last_not_of('0', result.size() - 1);
        if (lastSignificant != std::string::npos && lastSignificant >= decimalPoint) {
            result.erase(lastSignificant + 1, std::string::npos);
            if (result.back() == '.') {
                result.pop_back();  // 清除孤立的小数点
            }
        }
    }

    return result;
}

int AVSManager::getLonAndLat(std::string portCom) {
    this->longitude_ = "";
    this->latitude_ = "";
    if (portCom == "") {
        return 1;
    } else {
        serial::Serial my_serial(portCom, 9600, serial::Timeout::simpleTimeout(1000));
        if (my_serial.isOpen()) {
            spdlog::info("serial port open right.");
        } else {
            spdlog::warn("serial port:{} open failed.", portCom);
            throw std::exception("serial port open error");
        }
        std::string line;
        int timeout = 20;
        do {
            line = my_serial.readline();
        } while (!extractGNRMCLatLong(line, this->latitude_, this->longitude_) && timeout--);
        spdlog::info("READ the GNRMC str:{}", line);
        my_serial.close();
        if (this->latitude_ == "" || this->longitude_ == "" || line == "") {
            spdlog::warn("serial port {} reader error", portCom);
            return -1;
        }
        this->latitude_ = convertDegreesMinutesToDecimal(this->latitude_);
        this->longitude_ = convertDegreesMinutesToDecimal(this->longitude_);
    }
    return 0;
}
