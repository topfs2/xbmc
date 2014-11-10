#pragma once

/*
 *      Copyright (C) 2005-2014 Team XBMC
 *      http://xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "utils/params_check_macros.h"

#include <string>
#include <log4cplus/logger.h>  
#include <log4cplus/loggingmacros.h>  
  
// define some macros for simplicity
#define LOG_TRACE(logEvent)         log4cplus::LOG4CPLUS_TRACE(log4cplus::Logger::getRoot(), logEvent)  
#define LOG_DEBUG(logEvent)         log4cplus::LOG4CPLUS_DEBUG(log4cplus::Logger::getRoot(), logEvent)  
#define LOG_INFO(logEvent)          log4cplus::LOG4CPLUS_INFO(log4cplus::Logger::getRoot(), logEvent)  
#define LOG_WARN(logEvent)          log4cplus::LOG4CPLUS_WARN(log4cplus::Logger::getRoot(), logEvent)  
#define LOG_ERROR(logEvent)         log4cplus::LOG4CPLUS_ERROR(log4cplus::Logger::getRoot(), logEvent)  
#define LOG_FATAL(logEvent)         log4cplus::LOG4CPLUS_FATAL(log4cplus::Logger::getRoot(), logEvent)  

// ones we use in the code
#define LOGDEBUG   0
#define LOGINFO    1
#define LOGNOTICE  2
#define LOGWARNING 3
#define LOGERROR   4
#define LOGSEVERE  5
#define LOGFATAL   6
#define LOGNONE    7

class CLog
{
public:
  CLog();
  ~CLog(void);
  static bool Init(const std::string& path);
  static void Close();

  static void Log(int loglevel, PRINTF_FORMAT_STRING const char *format, ...) PARAM2_PRINTF_FORMAT;
  static void LogFunction(int loglevel, IN_OPT_STRING const char* functionName, PRINTF_FORMAT_STRING const char* format, ...) PARAM3_PRINTF_FORMAT;
#define LogF(loglevel,format,...) LogFunction((loglevel),__FUNCTION__,(format),##__VA_ARGS__)

private:
  static void LogString(int logLevel, const std::string& logString);
};
