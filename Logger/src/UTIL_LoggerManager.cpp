#include "UTIL_LoggerManager.h"

std::shared_ptr<UTIL_LoggerManager> UTIL_LoggerManager::logger_manager = nullptr;

UTIL_LoggerManager::UTIL_LoggerManager(UTIL_log_levels filter_level){
    this->log_queue = std::make_unique<UTIL_ThreadsafeQueue<std::string>>();
    this->log_filter = filter_level;
    this->running = this->logger_state::STOPPED;
}

UTIL_LoggerManager::~UTIL_LoggerManager(void){
    this->running = STOPPED;
    // Log message to wake up queue
    LogMessage(UTIL_LOG_INFO, "Log service shutting down");
    if(this->logging_service_thread.joinable()){
        this->logging_service_thread.join();
    }
}

std::shared_ptr<UTIL_LoggerManager> UTIL_LoggerManager::GetInstance(UTIL_log_levels filter_level){
    if(!logger_manager){
        logger_manager.reset(new UTIL_LoggerManager(filter_level));
    }
    return logger_manager;
}

std::shared_ptr<UTIL_LoggerManager> UTIL_LoggerManager::GetInstance(void){
    if(!logger_manager){
        logger_manager.reset(new UTIL_LoggerManager(DEFAULT_MASK));
    }
    return logger_manager;
}

void UTIL_LoggerManager::RegisterLogger(std::unique_ptr<UTIL_Logger> new_logger){
    this->registered_loggers.push_back(std::move(new_logger));
}

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

void UTIL_LoggerManager::LogMessage(UTIL_log_levels level, const char *format_string, ...){
    std::string formatted_string, final_string;
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

        final_string = this->GenerateTimestamp();
        final_string += " : " + this->level_to_string.at(level) + " : ";
        final_string += formatted_string;
        if(this->IsRunning()){
            this->log_queue->Enqueue(final_string);
        }
        else{
            printf("%s\n", formatted_string.c_str());
        }
    }
    else{
        va_end(args_copy);
        printf("LoggerManager : Error processing varargs for %s\n", format_string);
    }
}


void UTIL_LoggerManager::ProcessLogQueue(void){
    while(this->running){
        std::string log = this->log_queue->Dequeue(true);
        if(!log.empty()){
            for(const auto& logger : this->registered_loggers){
                //logger->LogMessage
            }
        }

    }
}


int UTIL_LoggerManager::IsRunning(void){
    return this->running;
}

std::string UTIL_LoggerManager::GenerateTimestamp(void){
    return std::string();
}