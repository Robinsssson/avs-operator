#ifndef __UTILITY_H__
#define __UTILITY_H__
#include <spdlog/spdlog.h>

#include <ctime>

#include "ErrorManager.h"

#define HANDLE_ERROR(code) ErrorManager::handleError(code, __FILE__, __LINE__, __FUNCTION__)
#define AVS_POLLSCAN_NO_DATA_AVAILABLE 0
namespace avs_util {
inline time_t getCurrentTimeT() { return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()); }
inline bool isApproximatelyEqual(double a, double b, double epsilon = 1e-9) { return std::abs(a - b) < epsilon; }
};  // namespace avs_util

#endif
