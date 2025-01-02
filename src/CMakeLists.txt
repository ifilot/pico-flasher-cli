cmake_minimum_required(VERSION 3.10)

# Set the project name
project(picoflash)

# store git hash
execute_process(
    COMMAND git log -1 --format=%h
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    OUTPUT_VARIABLE GIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# prepare configuration file
SET(PROGNAME "PICOFLASH")
SET(VERSION_MAJOR "0")
SET(VERSION_MINOR "1")
SET(VERSION_MICRO "0")
message(STATUS "Writing configuration file in: ${CMAKE_CURRENT_SOURCE_DIR}/config.h")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/config.h @ONLY)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add optimization flag
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

# Add the executable
add_executable(picoflash main.cpp serial.cpp flasher.cpp serialport.cpp)