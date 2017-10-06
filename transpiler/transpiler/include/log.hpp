//
//  log.hpp
//  transpiler
//
//  Created by Gautham Banasandra on 06/10/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <mutex>
#include <sstream>
#include <string>

enum LogLevel { logSilent, logInfo, logError, logWarning, logDebug, logTrace };

extern std::string appName;
extern LogLevel desiredLogLevel;
extern std::string workerID;

inline std::string NowTime();
static std::string LevelToString(LogLevel level);

extern std::ostringstream os;

extern void setAppName(std::string appName);
extern void setLogLevel(LogLevel level);
extern void setWorkerID(std::string ID);

extern std::mutex log_mutex;

static std::ostringstream &Logger(LogLevel level = logInfo) {
  time_t ctm;
  std::time(&ctm);
  char cts[128];
  std::strftime(cts, sizeof(cts), "%Y-%m-%dT%H:%M:%S%z", std::localtime(&ctm));
  std::string ts = cts;
  
  std::lock_guard<std::mutex> lock(log_mutex);
  os << ts.substr(0, ts.length() - 2) << ":" << ts.substr(ts.length() - 2);
  os << " " << LevelToString(level) << " ";
  os << "VWCP [" << appName << ":" << workerID << "] ";
  return os;
}

static std::string FlushLog() {
  std::string str = os.str();
  os.str(std::string());
  return str;
}

static std::string LevelToString(LogLevel level) {
  static const char *const buffer[] = {"[Silent]",  "[Info]",  "[Error]",
    "[Warning]", "[Debug]", "[Trace]"};
  return buffer[level];
}

static LogLevel LevelFromString(const std::string &level) {
  if (level == "SILENT")
    return logSilent;
  if (level == "INFO")
    return logInfo;
  if (level == "ERROR")
    return logError;
  if (level == "WARNING")
    return logWarning;
  if (level == "DEBUG")
    return logDebug;
  if (level == "TRACE")
    return logTrace;
  
  return logInfo;
}

#define LOG(level) std::cout

#endif
