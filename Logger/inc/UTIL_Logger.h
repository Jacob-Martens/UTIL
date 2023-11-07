#pragma once
/*
* FILE          : UTIL_Logger.h
* PROGRAMMER    : Jacob Martens
* CREATED       : 09-10-2023
* DESCRIPTION   :
* Abstract base class defining interface for specialized log message dispatchers
*/
#include <string>

class UTIL_Logger{
public:
    // Overridden by child classes to output log however it needs to
    virtual void WriteLog(std::string log) = 0;
};