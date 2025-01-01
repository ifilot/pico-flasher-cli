cmake_minimum_required(VERSION 3.10)

# Set the project name
project(picoflash)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add optimization flag
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

# Add the executable
add_executable(picoflash main.cpp serial.cpp)