#pragma once

#include "UTIL_Logger.h"

class UTIL_ConsoleLogger : public UTIL_Logger{
public:
    UTIL_ConsoleLogger(void);
    void WriteLog(std::string log) override;
};