#pragma once
/*
* FILE          : UTIL_DailyLogger.h
* PROGRAMMER    : Jacob Martens
* CREATED       : 06-11-2023
* DESCRIPTION   :
* Class to log messages to daily file. Implements the UTIL_Logger interface.
* Creates a log file from given filepath with an appended datestamp
*/
#include <string>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <exception>
#include <chrono>

#include "UTIL_Logger.h"

class UTIL_DailyLogger : public UTIL_Logger{
private:
    // Validates given filepath is within allowable size and checks R/W perms
    void ValidateFilepath(std::string filepath);

    // Formats a datestamp for appending to the filepath
    std::string GenerateDatestamp(void);

    // Base filepath string that datestamp is appended to
    std::string base_filepath;

    // Max filepath length, universal across Windows & Linux
    const int MAX_FILEPATH_SIZE = 256;
    // String size of generated datestamps
    const int DATESTAMP_SIZE = 16;
public:
    // Constructor
    UTIL_DailyLogger(std::string filepath);

    // Writes passed log to current daily file
    void WriteLog(std::string log) override;
};