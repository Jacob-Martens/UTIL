/*
* FILE          : UTIL_ConsoleLogger.cpp
* PROGRAMMER    : Jacob Martens
* CREATED       : 09-10-2023
* DESCRIPTION   :
* Class to log messages to console. Implements the UTIL_Logger interface
*/
#include "UTIL_ConsoleLogger.h"

// Default constructor
UTIL_ConsoleLogger::UTIL_ConsoleLogger(){
}

/*
* WriteLog
* ConsoleLogger version of WriteLog, prints formatted log to console
*/
void UTIL_ConsoleLogger::WriteLog(std::string log){
    printf("%s\n", log.c_str());
}