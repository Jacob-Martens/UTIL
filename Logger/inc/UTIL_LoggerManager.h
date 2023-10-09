#pragma once
#include <memory>
#include <chrono>
#include <sstream>
#include <string>
#include <thread>
#include <map>
#include <cstdarg>

#include "UTIL_ThreadsafeQueue.h"
#include "UTIL_Logger.h"

#ifdef __cplusplus
extern "C" {
#endif
// Available levels of log message
enum UTIL_log_levels {
    UTIL_LOG_ERR,
    UTIL_LOG_INFO,
    UTIL_LOG_DEBUG
};

void UTIL_Log(UTIL_log_levels level, const char* format_string, ...);

#ifdef ___cplusplus
}
#endif

class UTIL_LoggerManager {
public:   
    // Returns the active instance of the logger singleton
    // If the singleton hasn't been created, creates it with provided filter level
    static std::shared_ptr<UTIL_LoggerManager> GetInstance(UTIL_log_levels filter_level);
    // Returns the active instance of the logger singleton
    // If the singleton hasn't been created, creates it with default filter level of UTIL_LOG_INFO
    static std::shared_ptr<UTIL_LoggerManager> GetInstance(void);

    // Adds the newly created logger to the registered_loggers vector
    void RegisterLogger(std::unique_ptr<UTIL_Logger> new_logger);

    // Starts the background logging_service_thread, prevents the registering of any new loggers
    int StartLoggerService(void);

    // Accessor for the running variable, checks to see if the service is running
    int IsRunning(void);

    // Constructs the message struct and enqueues it for the background thread
    void LogMessage(UTIL_log_levels level, const char* format_string, ...);

    // Wakes up blocking call on background thread so system can shutdown
    ~UTIL_LoggerManager(void);

private:
    // Private constructor
    // Sets log_filter to the provided filter_level
    UTIL_LoggerManager(UTIL_log_levels filter_level);
    // This is a singleton class
    static std::shared_ptr<UTIL_LoggerManager> logger_manager;

    // Returns a properly formatted timestamp
    std::string GenerateTimestamp(void);

    // Function used by background thread
    // Blocks on message queue until log comes in, processes
    void ProcessLogQueue(void);

    // Data-members
    int running;
    UTIL_log_levels log_filter;
    std::thread logging_service_thread;
    std::unique_ptr<UTIL_ThreadsafeQueue<std::string>> log_queue;
    std::vector<std::unique_ptr<UTIL_Logger>> registered_loggers;

    enum logger_state{
        STOPPED,
        RUNNING
    };

    const std::map<UTIL_log_levels, std::string> level_to_string = {
        {UTIL_LOG_ERR, "ERROR"},
        {UTIL_LOG_INFO, "INFO"},
        {UTIL_LOG_DEBUG, "DEBUG"}
    };

    const static UTIL_log_levels DEFAULT_MASK = UTIL_LOG_INFO;
};
}