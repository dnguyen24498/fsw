cmake_minimum_required(VERSION 3.0)

project(fsw VERSION 1.0.1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -g -pthread -lrt -Wall -Wextra -DLOG_FILE")
set(PROJECT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

set(LOG_DIRECTORY ${PROJECT_PATH}/build/fsw_log.txt)

set(BINARY_PATH ${PROJECT_PATH}/build/fsw)

set(PORT_NAME /dev/virtualcom0)
set(PORT_BAUDRATE 115200)
set(ENGINEERING_PORT_BAUDRATE 460800)
set(UPDATE_PORT_BAUDRATE 921600)

set(FRAME_BUFFER_SIZE 1024)

configure_file(
    ${PROJECT_PATH}/src/utils/Version.h.in
    ${PROJECT_PATH}/src/utils/Version.h @ONLY
)

configure_file(
    ${PROJECT_PATH}/src/Configuration.inc.in
    ${PROJECT_PATH}/src/Configuration.inc @ONLY
)

configure_file(
    ${PROJECT_PATH}/files/fsw.service.in
    ${PROJECT_PATH}/files/fsw.service @ONLY
)

include_directories(
    src
    src/utils
    src/platform
    src/services/connectivity
    src/services/factory
    src/services/updateengine
    src/services/engineering
)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
