/*
* FILE          : UTIL_LoggerManager.cpp
* PROGRAMMER    : Jacob Martens
* CREATED       : 09-10-2023
* DESCRIPTION   :
* Background logging library built on a blocking queue and a singleton mediator
* with registered abstract logging dispatchers
*/
#include "UTIL_LoggerManager.h"

// Singleton instance
std::shared_ptr<UTIL_LoggerManager> UTIL_LoggerManager::logger_manager = nullptr;

/*
* Private Constructor
* Sets log filter to filter_level, creates the blocking queue
* and sets logger_state to stopped. Invoked once first time GetInstance is called
*/
UTIL_LoggerManager::UTIL_LoggerManager(UTIL_log_levels filter_level){
    this->log_queue = std::make_unique<UTIL_ThreadsafeQueue<std::string>>();
    this->log_filter = filter_level;
    this->running = this->logger_state::STOPPED;
}

/*
* Destructor
* Logs message to wake up background thread and marks process for stopping, joining threads
*/
UTIL_LoggerManager::~UTIL_LoggerManager(void){
    this->running = STOPPING;
    // Log message to wake up queue
    LogMessage(UTIL_LOG_INFO, "Log service shutting down");
    if(this->logging_service_thread.joinable()){
        this->logging_service_thread.join();
    }
}

/*
* GetInstance
* Overloaded GetInstance, if the logger is not instantiated, does so with passed filter level
*/
std::shared_ptr<UTIL_LoggerManager> UTIL_LoggerManager::GetInstance(UTIL_log_levels filter_level){
    if(!logger_manager){
        logger_manager.reset(new UTIL_LoggerManager(filter_level));
    }
    return logger_manager;
}

/*
* GetInstance
* Instantiates the logger if not already created with a filter level of INFO
*/
std::shared_ptr<UTIL_LoggerManager> UTIL_LoggerManager::GetInstance(void){
    if(!logger_manager){
        logger_manager.reset(new UTIL_LoggerManager(DEFAULT_MASK));
    }
    return logger_manager;
}

/*
* Register Logger
* Adds a successfully constructed UTIL_Logger to the registered logger queue
*/
void UTIL_LoggerManager::RegisterLogger(std::unique_ptr<UTIL_Logger> new_logger){
    this->registered_loggers.push_back(std::move(new_logger));
}

/*
* StartLoggerService
* Starts the background thread for processing logged messages
* Prevents the registering of additional UTIL_Loggers
*/
int UTIL_LoggerManager::StartLoggerService(void){
    int retCode = -1;
    if(this->registered_loggers.size() > 0 && !this->running){
        this->running = RUNNING;
        std::thread start(&UTIL_LoggerManager::ProcessLogQueue, this);
        this->logging_service_thread = move(start);
        retCode = 0;
    }
    else if (this->registered_loggers.size() == 0) {
        printf("LoggerManager : Service not started, no registered loggers\n");
    }
    else{
        printf("LoggerManager : Service already started\n");
    }
    return retCode;
}

/*
* UTIL_Log
* Function to enqueue message in log queue
* Callable from C code
*/
void UTIL_Log(UTIL_log_levels level, const char* format_string, ...){
    std::shared_ptr<UTIL_LoggerManager> logger = UTIL_LoggerManager::GetInstance();
    std::string formatted_string;
    va_list args, args_copy;
    long long len;
    
    va_start(args, format_string);
    va_copy(args_copy, args);

    len = vsnprintf(nullptr, 0, format_string, args);
    va_end(args);
    
    if(len >= 0){
        formatted_string.resize(len);
        vsnprintf(&formatted_string[0], len+1, format_string, args_copy);
        va_end(args_copy);
        logger->LogMessage(level, formatted_string.c_str());
    }
    else{
        va_end(args_copy);
        printf("LoggerManager : Error processing varargs for %s\n", format_string);
    }
    
}

/*
* LogMessage
* Method to enqueue message in log queue
* If logger isn't active, prints to console
*/
void UTIL_LoggerManager::LogMessage(UTIL_log_levels level, const char *format_string, ...){
    std::string formatted_string, time_string, level_string; 
    std::stringstream final_string;
    va_list args, args_copy;
    long long len;
    
    // Determine required size for formatted string
    va_start(args, format_string);
    va_copy(args_copy, args);

    len = vsnprintf(nullptr, 0, format_string, args);
    va_end(args);
    
    if(len >= 0){
        // Print the formatter and arguments to formatted string
        formatted_string.resize(len);
        vsnprintf(&formatted_string[0], len+1, format_string, args_copy);
        va_end(args_copy);
        
        // Generate timestamp, get level string
        time_string = this->GenerateTimestamp();
        level_string = this->level_to_string.at(level);

        // Assemble final string
        final_string << time_string << " : " << level_string << " : " << formatted_string;

        // Enqueue or print 
        if(this->running != STOPPED){
            this->log_queue->Enqueue(final_string.str());
        }
        else{
            printf("%s\n", final_string.str().c_str());
        }
    }
    else{
        va_end(args_copy);
        printf("LoggerManager : Error processing varargs for %s\n", format_string);
    }
}

/*
* ProcessLogQueue
* Worker function in background thread
* Blocking call to dequeue, dispatches to registered loggers
*/
void UTIL_LoggerManager::ProcessLogQueue(void){
    while(this->running == RUNNING){
        std::string log = this->log_queue->Dequeue(true);
        if(!log.empty()){
            for(const auto& logger : this->registered_loggers){
                logger->WriteLog(log);
            }
        }
    }
}

/*
* GenerateTimestamp
* Creates a timestamp string for the moment it is called
* Format : YYYY-MM-DD HH:MM:SS.sss
*/
std::string UTIL_LoggerManager::GenerateTimestamp(void){
    // Get clock values
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count() % 1000;
    auto t = std::time(0);
    auto now = std::localtime(&t);

    // Create string large enough to fit formatted timestamp
    std::string timestamp;
    timestamp.resize(sizeof("9999-12-31 29:59:59.9999"));
    
    sprintf(&timestamp[0], "%04d-%02d-%02d %02d:%02d:%02d.%03d",
        now->tm_year + 1900,
        now->tm_mon + 1,
        now->tm_mday,
        now->tm_hour,
        now->tm_min,
        now->tm_sec,
        (int)millis);
    
    timestamp.erase(std::find(timestamp.begin(), timestamp.end(), '\0'), timestamp.end());

    return timestamp;
}