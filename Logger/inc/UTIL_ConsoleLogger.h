#pragma once
/*
* FILE          : UTIL_ConsoleLogger.h
* PROGRAMMER    : Jacob Martens
* CREATED       : 09-10-2023
* DESCRIPTION   :
* Class to log messages to console. Implements the UTIL_Logger interface
*/
#include "UTIL_Logger.h"

class UTIL_ConsoleLogger : public UTIL_Logger{
public:
    // Constructor
    UTIL_ConsoleLogger(void);
    
    // Writes passed log message to console
    void WriteLog(std::string log) override;
};