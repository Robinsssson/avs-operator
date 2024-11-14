#include <windows.h>

#include <argparse/argparse.hpp>
#include <atomic>
#include <chrono>
#include <fstream>
#include <ios>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "AVSManager.h"
#include "spdlog/spdlog.h"
#include <spdlog/sinks/basic_file_sink.h>

std::atomic<bool> running_flag(true), exit_thread_flag(false);
static const std::string versionOfAvsOperator = "1.0.0";
std::shared_ptr<spdlog::logger> file_logger;
int main(int argc, const char *argv[]) {
    argparse::ArgumentParser program("avs-operator",versionOfAvsOperator);
    
    program.add_argument("-o", "--output").help("set the output file path").default_value(std::string("output_file.txt"));
    program.add_argument("-g", "--logging").help("set the logging file path").default_value(std::string("None"));
    std::string outputFile, loggingFile;
    try {
        program.parse_args(argc, argv);
        // if (program["version"] == true) {
        //     std::cout << "Version " << versionOfAvsOperator << std::endl;
        // }
        outputFile = program.get<std::string>("output");
        loggingFile = program.get<std::string>("logging");
        if (loggingFile != "None" )
        {
            file_logger = spdlog::basic_logger_mt("logger", loggingFile);
            spdlog::set_default_logger(file_logger);
        }
        spdlog::info("the output file of log is in {}", outputFile);
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    AVSManager manage;
    auto ret = manage.findDevice();
    manage.activateDevice(ret - 1);
    manage.measurePerpare(ret - 1);
    auto ret_val = manage.measureData(ret - 1);
    std::fstream file(outputFile, std::ios_base::out | std::ios_base::trunc);
    if (!file.is_open()) {
        spdlog::error("message:error to open log file. function {}, line {}", __FUNCTION__, __LINE__);
        return -1;
    }
    for (auto &v : ret_val) {
        file << v << std::endl;
    }
    file.close();
    return 0;
}
