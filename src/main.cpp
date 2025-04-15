#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>
#include <windows.h>

#include <argparse/argparse.hpp>
#include <atomic>
#include <chrono>
#include <ctime>
#include <exception>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <typeinfo>

#include "AVSManager.h"
#include "Utility.h"
#include "fmt/chrono.h"
#include "spdlog/common.h"

static const std::string versionOfAvsOperator = "1.0.0";

std::atomic<double> globalAngleOfMotor(90);
std::atomic<AVSManager::AdjustMethod> globalMethod(AVSManager::AdjustMethod::average);

std::shared_ptr<spdlog::logger> file_logger;
std::unique_ptr<AVSManager> avsManager;

void timerHookFunction(int, int, const std::filesystem::path &, std::string &, bool);

int main(int argc, const char *argv[]) {
    argparse::ArgumentParser program("avs-operator", versionOfAvsOperator);

    program.add_argument("-o", "--output")
        .default_value(std::string("."))
        .metavar("file_path")
        .help("set the output file path");

    program.add_argument("-g", "--log")
        .default_value(std::string("None"))
        .metavar("log_file_path")
        .help("set the log file path");

    program.add_argument("-t", "--measuretime")
        .default_value(1)
        .metavar("number")
        .help("set the measure times")
        .scan<'i', int>();

    program.add_argument("-i", "--integraltime")
        .default_value(5)
        .metavar("number")
        .help("set the integral times")
        .scan<'i', int>();

    program.add_argument("-n", "--integralnumber")
        .default_value(50)
        .metavar("number")
        .help("set the integral number")
        .scan<'i', int>();

    program.add_argument("-a", "--angle")
        .default_value(90)
        .metavar("number")
        .help("angle setting for this")
        .scan<'i', int>();

    program.add_argument("-m", "--method")
        .default_value(std::string("average"))
        .metavar("'average'|'maximum'")
        .help("set method average or maximum");

    program.add_argument("--disable-adjust")
        .default_value(false)
        .metavar("boolean")
        .help("set true will adjust value")
        .implicit_value(true);

    program.add_argument("-p", "--port").default_value(std::string("")).metavar("COMx").help("set the com of GPS");

    std::string outputFilePathStr, loggingFile, portCom;
    int measureTime, integralTime, integralNumber;
    bool disableAdjust;
    try {
        program.parse_args(argc, argv);
    } catch (const std::exception &e) {
        fmt::println(std::cerr, e.what());
        return 1;
    }
    try {
        outputFilePathStr = program.get<std::string>("output");
    } catch (const std::exception &e) {
        fmt::println(std::cerr, e.what());
        return 2;
    }
    try {
        loggingFile = program.get<std::string>("log");
    } catch (const std::exception &e) {
        fmt::println(std::cerr, e.what());
        return 3;
    }
    try {
        measureTime = program.get<int>("measuretime");
    } catch (const std::exception &e) {
        fmt::println(std::cerr, e.what());
        return 4;
    }
    try {
        integralTime = program.get<int>("integraltime");
    } catch (const std::exception &e) {
        fmt::println(std::cerr, e.what());
        return 5;
    }
    try {
        integralNumber = program.get<int>("integralnumber");
    } catch (const std::exception &e) {
        fmt::println(std::cerr, e.what());
        return 6;
    }
    try {
        globalAngleOfMotor.store(program.get<int>("angle"), std::memory_order_relaxed);
    } catch (const std::exception &e) {
        fmt::println(std::cerr, e.what());
        return 7;
    }
    try {
        std::string method_str = program.get<std::string>("method");
        AVSManager::AdjustMethod method;
        if (method_str == "average")
            method = AVSManager::AdjustMethod::average;
        else if (method_str == "maximum")
            method = AVSManager::AdjustMethod::maximum;
        else
            throw std::bad_cast();
        globalMethod.store(method, std::memory_order_relaxed);
    } catch (const std::exception &e) {
        fmt::println(std::cerr, e.what());
        return 8;
    }

    try {
        portCom = program.get<std::string>("port");
    } catch (const std::exception &e) {
        fmt::println(std::cerr, e.what());
        return 9;
    }

    try {
        disableAdjust = program.get<bool>("--disable-adjust");
    } catch (const std::exception &e) {
        fmt::println(std::cerr, e.what());
        return 10;
    }

    if (loggingFile != "None") {
        file_logger = spdlog::basic_logger_mt("logger", loggingFile);
        spdlog::set_default_logger(file_logger);
    }
    
    spdlog::set_level(spdlog::level::debug);

    spdlog::info("the output file of log is in {}", outputFilePathStr);
    auto dirName = fmt::format("{:%Y-%m-%d}", fmt::localtime(avs_util::getCurrentTimeT()));
    std::filesystem::path outputFilePath(outputFilePathStr);
    outputFilePath = outputFilePathStr / std::filesystem::path(dirName);
    std::filesystem::create_directories(outputFilePath);
    spdlog::info("entry create {} path", outputFilePath.string());
    avsManager = std::make_unique<AVSManager>(0, 300, 460, std::string("Huai Bei"));
    auto numberID = avsManager->findDevice() - 1;
    if (numberID < 0) {
        spdlog::error("plz check avs-line is linked?");
        return -1;
    }
    avsManager->activateDevice(numberID);
    try {
        while (measureTime--) {
            timerHookFunction(integralTime, integralNumber, outputFilePath, portCom, disableAdjust);
        }
    } catch (std::exception &e) {
        spdlog::error("get a unprocess error {}", e.what());
        return -7;
    }
    return 0;
}

void timerHookFunction(int timeVal, int averageNumber, const std::filesystem::path &outputFilePath,
                       std::string &portCom, bool disableAdjust) {
    static unsigned int timeEntry = 0;
    int tick = timeVal * averageNumber;

    spdlog::info("Start Adjust Measure");

    time_t inputTimeT = avsManager->measurePerpare(avsManager->getActivateID(), timeVal, averageNumber);
    std::this_thread::sleep_for(std::chrono::milliseconds(tick));
    if (!disableAdjust) {
        auto adjustData = avsManager->measureData(avsManager->getActivateID());
        timeVal = avsManager->adjustVal(std::get<0>(adjustData), globalAngleOfMotor, globalMethod);
    
        spdlog::info("Adjust integral-Time to {}", timeVal);
        spdlog::info("Start Right Measure");
    
        inputTimeT = avsManager->measurePerpare(avsManager->getActivateID(), timeVal, averageNumber);
        std::this_thread::sleep_for(std::chrono::milliseconds(tick));
    } else {
        avsManager->setAngle(globalAngleOfMotor);
    }
    auto retData = avsManager->measureData(avsManager->getActivateID());
    avsManager->saveDataInFile(outputFilePath, std::get<0>(retData), inputTimeT, std::get<1>(retData), portCom);

    spdlog::info("Save the {} times Measure in PATH {}", ++timeEntry, outputFilePath.string());
}
