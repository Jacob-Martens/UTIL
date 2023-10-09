cmake_minimum_required(VERSION 3.1)
include(CMakePrintHelpers)
project(UTIL LANGUAGES CXX)

file(GLOB sem_files ${PROJECT_SOURCE_DIR}/Semaphore/src/*.cpp)
file(GLOB queue_files ${PROJECT_SOURCE_DIR}/ThreadsafeQueue/src/*.cpp)

list(APPEND all_files ${sem_files} ${queue_files})
#list(APPEND all_files ${queue_files})

cmake_print_variables(all_files)

add_library(util STATIC ${all_files})

target_include_directories(util PUBLIC ${PROJECT_SOURCE_DIR}/Semaphore/inc ${PROJECT_SOURCE_DIR}/ThreadsafeQueue/inc)