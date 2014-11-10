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

#include "log.h"
#include "system.h"
#include "threads/SingleLock.h"
#include "threads/Thread.h"
#include "utils/StringUtils.h"
#include "CompileInfo.h"

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

using namespace log4cplus;

static Logger logger = Logger::getRoot();

CLog::CLog()
{}

CLog::~CLog()
{}

bool CLog::Init(const std::string& path)
{
/*
  CSingleLock waitLock(s_globals.critSec);

  // the log folder location is initialized in the CAdvancedSettings
  // constructor and changed in CApplication::Create()

  std::string appName = CCompileInfo::GetAppName();
  StringUtils::ToLower(appName);
  return s_globals.m_platform.OpenLogFile(path + appName + ".log", path + appName + ".old.log");
*/
  return true;
}

void CLog::Close()
{
}

void CLog::Log(int loglevel, const char *format, ...)
{
  va_list va;
  va_start(va, format);
  std::string logstring = StringUtils::FormatV(format, va);
  va_end(va);

  LogString(loglevel, logstring);
}

void CLog::LogFunction(int loglevel, const char* functionName, const char* format, ...)
{
  std::string fNameStr;
  if (functionName && functionName[0])
    fNameStr.assign(functionName).append(": ");
  va_list va;
  va_start(va, format);
  LogString(loglevel, fNameStr + StringUtils::FormatV(format, va));
  va_end(va);
}

void CLog::LogString(int logLevel, const std::string& logString)
{
  LOG4CPLUS_DEBUG(logger, logString);
}
