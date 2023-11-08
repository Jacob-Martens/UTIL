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

int UnitTest_InvalidUse(void);
int UnitTest_LoggerConstruction(void);
void UnitTest_LoggingMessages(void);

int main(void){
    int retCode = 0;

    int failed_tests = 0;
    // Create the log singleton with middle filter option for testing
    std::shared_ptr<UTIL_LoggerManager> logger = UTIL_LoggerManager::GetInstance(UTIL_LOG_INFO);

    printf("UTIL_LoggerManager Test Harness\n===============================\n\n");

    // Test 1
    printf("Test 1 : Performing invalid actions...\n");
    retCode = UnitTest_InvalidUse(); 
    printf("Test 1 : Complete, %d failed\n\n", retCode);
    failed_tests += retCode; 

    // Test 2
    printf("Test 2 : Constructing various loggers...\n");
    retCode = UnitTest_LoggerConstruction();
    printf("Test 2 : Complete, %d failed\n\n", retCode);
    failed_tests += retCode;

    // Start the service
    logger->StartLoggerService();

    // Test 3
    printf("Test 3 : Log various messages, all require manual verification...\n");
    UnitTest_LoggingMessages();
    printf("Test 3 : Complete, press enter when ready\n\n");

    getc(stdin);

    printf("UTIL_LoggerManager : Tests complete, %d failed\n", failed_tests);

    return 0;
}

// Tests 1 : Invalid use of the logging service
int UnitTest_InvalidUse(void){
    std::shared_ptr<UTIL_LoggerManager> logger = UTIL_LoggerManager::GetInstance();
    int retCode = 0;

    // Test 1.1 : Attempt to log message without service instantiated - c-style
    printf("\t1.1: Logging message without service running - c-style\n");
    UTIL_Log(UTIL_LOG_INFO, "Test Log Message");
    printf("\t1.1: Manual validation - Should be properly formatted \"Test Log Message\"\n"); 

    // Test 1.2 : Attempt to log message without service instantiated - c++-style
    printf("\t1.2: Logging message without service running - c++-style\n");
    logger->LogMessage(UTIL_LOG_INFO, "Test Log Message");
    printf("\t1.2: Manual validation - Should be properly formatted \"Test Log Message\"\n");

    // Test 1.3 : Attempt to start service with no registered loggers active
    printf("\t1.3: Starting service with no registered loggers\n");
    retCode = logger->StartLoggerService(); 
    if (retCode == 0) {
        printf("\t1.3: Failed, function returned %d, expected -1\n", retCode);
        retCode = 1;
    }
    else {
        printf("\t1.3: Passed\n");
        retCode = 0;
    }

    return retCode;
}

// Tests 2 : Constructing log message dispatchers
int UnitTest_LoggerConstruction(void){
    std::shared_ptr<UTIL_LoggerManager> logger = UTIL_LoggerManager::GetInstance();
    int retCode = 0;

    // Loggers that will be tested
    std::unique_ptr<UTIL_Logger> valid_console;
    std::unique_ptr<UTIL_Logger> valid_daily;
    std::unique_ptr<UTIL_Logger> invalid_daily;
    //std::shared_ptr<UTIL_Logger> valid_rolling;
    //std::shared_ptr<UTIL_Logger> invalid_rolling;


    // Create a valid Console Logger
    valid_console = std::make_unique<UTIL_ConsoleLogger>();
    logger->RegisterLogger(std::move(valid_console));

    // Test 2.1 : Creating a valid DailyLogger
    printf("\t2.1: Creating a valid DailyLogger...\n");
    try {
        valid_daily = std::make_unique<UTIL_DailyLogger>("./temp");
        logger->RegisterLogger(std::move_if_noexcept(valid_daily));
        printf("\t2.1: Passed, logger successfully created\n");
    }
    catch (const std::runtime_error& err) {
        retCode++;
        printf("\t2.1: Failed, %s\n", err.what());
    }

    // Test 2.2 : Creating invalid DailyLogger - Nonexistant path
    printf("2.2: Creating an invalid DailyLogger...\n");
    try {
        invalid_daily = std::make_unique<UTIL_DailyLogger>("/not/a/path"); 
        retCode++;
        printf("\t2.2: Failed, logger successfully created\n");
    }
    catch (const std::runtime_error& err) {
        printf("\t2.2: Passed, %s\n", err.what());
    }

    // Test 2.3 : Creating invalid DailyLogger - Path too long
    printf("\t2.3: Creating an invalid DailyLogger...\n");
    try {
        invalid_daily = std::make_unique<UTIL_DailyLogger>("/not/a/path");
        retCode++;
        printf("\t2.3: Failed, logger successfully created\n");
    }
    catch (const std::runtime_error& err) {
        printf("\t2.3: Passed, %s\n", err.what());
    }

    return retCode;
}

// Tests 3 : Various message logging
void UnitTest_LoggingMessages(void){
    std::shared_ptr<UTIL_LoggerManager> logger = UTIL_LoggerManager::GetInstance();
    
    // Test 3.1 : Valid log message
    printf("\t3.1: Valid log message\n");
    logger->LogMessage(UTIL_LOG_INFO, "Test Log Message");
    printf("\t3.1: Expected Output : Test Log Message : Press enter when verified\n");
    getc(stdin);

    // Test 3.2 : Parameterized log message
    printf("\t3.2: Parameterized log message\n"); 
    logger->LogMessage(UTIL_LOG_INFO, "Test Log Message %d %s", 10, "<-this should read 10");
    printf("\t3.2: Expected Output : Test Log Message 10 <-this should read 10 : Press enter when verified\n");
    getc(stdin); 
     
    // Test 3.3 : Incredibly long log message
    printf("\t3.3: Incredibly long log message\n");
    logger->LogMessage(UTIL_LOG_INFO, "Lorem ipsum dolor sit amet, consectetur adipiscing elit. "
        "Praesent eget sapien est. Pellentesque aliquet et massa quis gravida. Proin quis rutrum elit. "
        "Aenean dictum ultrices lobortis.Vestibulum purus erat, luctus sit amet urna vel, vestibulum luctus dolor. "
        "Quisque ullamcorper justo dictum sapien consequat facilisis.Mauris vitae ex in orci laoreet facilisis."
        "Pellentesque ultricies ullamcorper ornare.Curabitur nec odio in enim condimentum ultricies.Vestibulum non libero nec neque pulvinar sit."); 
    printf("\t3.3: Expected Output : 500 bytes of Lorem Ipsum : Press enter when verified\n"); 
    getc(stdin); 

    // Test 3.4 : Rapid-Fire Messages
    printf("\t3.4: Rapid-Fire Messages\n");
    for (int i = 0; i < 1000; i++) {
        logger->LogMessage(UTIL_LOG_INFO, "Rapid-Fire Message %d", i + 1);
    }
    printf("\t3.4: Expected Output : Final message should read Rapid-Fire Message 1000 : Press enter when verified\n");
    getc(stdin);

    // Test 3.5 : Message below filter level
    printf("\t3.4: Log message below filter level\n");
    logger->LogMessage(UTIL_LOG_DEBUG, "Should not be seen");
    printf("\t3.5: Expected Output : No output : Press enter when verified\n");
    getc(stdin); 
}
