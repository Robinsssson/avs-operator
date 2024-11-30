#ifndef __UTILITY_H__
#define __UTILITY_H__
#include <spdlog/spdlog.h>

#include "ErrorManager.h"


#define LOG_ERROR(code) ErrorManager::logError(code, __FILE__, __LINE__, __FUNCTION__)
#endif
