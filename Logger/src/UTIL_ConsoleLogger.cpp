#include "UTIL_ConsoleLogger.h"

UTIL_ConsoleLogger::UTIL_ConsoleLogger(){
}

void UTIL_ConsoleLogger::WriteLog(std::string log){
    printf("%s\n", log.c_str());
}