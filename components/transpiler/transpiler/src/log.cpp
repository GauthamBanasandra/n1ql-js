//
//  log.cpp
//  transpiler
//
//  Created by Gautham Banasandra on 06/10/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#include "log.hpp"

#include <iostream>
#include <sstream>

std::ostringstream os;

std::string appName = "";
LogLevel desiredLogLevel = LogLevel(0);
std::string workerID = "";

std::mutex log_mutex;

void setAppName(std::string app) { appName = app; }

void setLogLevel(LogLevel level) { desiredLogLevel = level; }

void setWorkerID(std::string wID) { workerID = wID; }
