cmake_minimum_required(VERSION 3.1)
include(CMakePrintHelpers)
project(UTIL LANGUAGES CXX)

file(GLOB sem_files ${PROJECT_SOURCE_DIR}/Semaphore/src/*.cpp)
file(GLOB queue_files ${PROJECT_SOURCE_DIR}/ThreadsafeQueue/src/*.cpp)
file(GLOB log_files ${PROJECT_SOURCE_DIR}/Logger/src/*.cpp)

list(APPEND all_files ${sem_files} ${queue_files} ${log_files})

cmake_print_variables(all_files)

add_library(util STATIC ${all_files})
add_executable(logger_test_harness ${PROJECT_SOURCE_DIR}/Tests/logger_test_harness.cpp)

target_include_directories(util PUBLIC 
    ${PROJECT_SOURCE_DIR}/Semaphore/inc 
    ${PROJECT_SOURCE_DIR}/ThreadsafeQueue/inc
    ${PROJECT_SOURCE_DIR}/Logger/inc
)

target_include_directories(logger_test_harness PUBLIC 
    ${PROJECT_SOURCE_DIR}/Semaphore/inc 
    ${PROJECT_SOURCE_DIR}/ThreadsafeQueue/inc
    ${PROJECT_SOURCE_DIR}/Logger/inc
)

target_link_libraries(logger_test_harness util)