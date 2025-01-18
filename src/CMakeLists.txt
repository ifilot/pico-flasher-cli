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
SET(VERSION_MAJOR "1")
SET(VERSION_MINOR "0")
SET(VERSION_MICRO "0")
message(STATUS "Writing configuration file in: ${CMAKE_CURRENT_SOURCE_DIR}/config.h")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/config.h @ONLY)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)
pkg_check_modules(UDEV REQUIRED libudev)

# Include directories
include_directories(${CURL_INCLUDE_DIRS} ${UDEV_INCLUDE_DIRS})

# Add optimization flag
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

# Add the executable
add_executable(picoflash main.cpp serial.cpp flasher.cpp serialport.cpp)
target_link_libraries(picoflash OpenSSL::SSL OpenSSL::Crypto ${CURL_LIBRARIES} ${UDEV_LIBRARIES})

# Define where to install the executable
install(TARGETS picoflash
    RUNTIME DESTINATION bin # For executables
)