#pragma once
#include <string>

class UTIL_Logger{
public:
    virtual void WriteLog(std::string log) = 0;
};