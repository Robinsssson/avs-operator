#ifndef __ERROR_MANAER_H
#define __ERROR_MANAER_H
#include <string>
#include <unordered_map>

#include "spdlog/spdlog.h"

class ErrorManager {
   public:
    // 错误码到错误信息的映射
    static const std::unordered_map<int, std::string> errorMap;

    // 获取错误信息函数
    static std::string getErrorMessage(int errorCode) {
        auto it = errorMap.find(errorCode);
        if (it != errorMap.end()) {
            return it->second;
        } else {
            return "Unknown error code: " + std::to_string(errorCode);
        }
    }
    static bool handleError(int errorCode, const char* file, int line, const char* function) {
        if (errorCode >= 0) return false;
        auto it = errorMap.find(errorCode);
        if (it != errorMap.end()) {
            spdlog::error("[{}:{}] [{}] Error code {}: {}", file, line, function, errorCode, it->second);
        } else {
            spdlog::error("[{}:{}] [{}] Unknown error code {}.", file, line, function, errorCode);
        }
        return true;
    }
};

#endif