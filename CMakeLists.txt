cmake_minimum_required(VERSION 3.25)
project(cc-wc-tool-c 
        DESCRIPTION "Build your own version of the Unix command line tool wc"    
        LANGUAGES C)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

add_executable(ccwc src/ccwc.c)
