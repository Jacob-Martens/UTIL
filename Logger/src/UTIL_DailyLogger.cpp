/*
* FILE          : UTIL_DailyLogger.cpp
* PROGRAMMER    : Jacob Martens
* CREATED       : 06-11-2023
* DESCRIPTION   :
* Class to log messages to daily file. Implements the UTIL_Logger interface.
* Creates a log file from given filepath with an appended datestamp
*/
#include "UTIL_DailyLogger.h"

/*
* Constructor
* Validates the given filepath and returns constructed object
* Throws std::runtime_error if unable to validate filepath
*/
UTIL_DailyLogger::UTIL_DailyLogger(const std::string& filepath){
    try{
        ValidateFilepath(filepath);
        this->base_filepath = filepath;
    }
    catch (const std::runtime_error& err){
        std::string err_str = "Daily Logger not created : ";
        err_str += err.what();
        throw std::runtime_error(err_str);
    }
}

/*
* ValidateFilepath
* Tests that given filepath is valid
* Verifies filepath is within max size, and has R/W permissions
* Throws std::runtime_error is unable to validate
*/
void UTIL_DailyLogger::ValidateFilepath(const std::string& filepath) const{
    std::string temp_path;
    std::ofstream stream;

    // Test 1 : Verify generated file length doesn't exceed max
    if((filepath.size() + DATESTAMP_SIZE) < MAX_FILEPATH_SIZE){
        // Test 2 : Validate R/W perms for requested file
        temp_path = filepath + ".temp";
        stream.open(temp_path);
        if(stream.is_open()){
            // Cleanup remove temporary files
            stream.close();
            std::remove(temp_path.c_str());
        }
        else{
            std::string err_str = "Unable to R/W to filepath " + filepath + ".";
            throw std::runtime_error(err_str);
        }
    }
    else{
        std::string err_str = "Filepath too long, max = 257, requested = " + std::to_string(filepath.size() + DATESTAMP_SIZE) + ".";
        throw std::runtime_error(err_str);
    }
}

/*
* WriteLog
* DailyLogger implementation of WriteLog
* Opens the daily file and appends log to it
*/
void UTIL_DailyLogger::WriteLog(std::string log){
    std::ofstream out_stream;
    // Generate current filename, by base name and datestamp
    std::string current_file = this->base_filepath;
    current_file += this->GenerateDatestamp();

    // Open the file, check if successful
    out_stream.open(current_file, std::ios::out | std::ios::app | std::ios::binary);
    if(out_stream.is_open()){
        // Write log string and newline to file
        out_stream.write(log.c_str(), log.size()).write("\n", 1);

        // If successful, close file. Else, garbage collection should take care of
        if(out_stream.good()){
            out_stream.close();
        }
        else{
            printf("UTIL_DailyLogger : Error occured in writing to daily log\n");
        }
    }
}

/*
* GenerateDatestamp
* Creates & formats a datestamp to append to filename
* Formatted as -YYYY-MM-DD.log
*/
std::string UTIL_DailyLogger::GenerateDatestamp(void) const{
    std::string formatted_datestamp;

    time_t now = time(0);
    tm const * datestamp = localtime(&now);
    formatted_datestamp.resize(DATESTAMP_SIZE);
    strftime(&formatted_datestamp[0], sizeof(formatted_datestamp), "-%Y-%m-%d.log", datestamp);

    return formatted_datestamp;
}