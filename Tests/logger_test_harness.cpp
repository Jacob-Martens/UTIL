/*
* FILE          : logger_test_harness.cpp
* PROGRAMMER    : Jacob Martens
* CREATED       : 09-10-2023
* DESCRIPTION   :
* C++ test harness for validating the UTIL_LoggerManager system
*/
#include "UTIL_LoggerManager.h"
#include "UTIL_DailyLogger.h"
#include "UTIL_ConsoleLogger.h"

int main(void){
    std::shared_ptr<UTIL_LoggerManager> logger = UTIL_LoggerManager::GetInstance(UTIL_LOG_DEBUG);

    logger->RegisterLogger(std::make_unique<UTIL_ConsoleLogger>());
    logger->StartLoggerService();

    logger->LogMessage(UTIL_LOG_INFO, "Test log message");

    getc(stdin);

    return 0;
}
