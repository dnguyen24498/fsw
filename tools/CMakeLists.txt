cmake_minimum_required(VERSION 3.0)

project(fsw_update VERSION 1.0.1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -g -pthread -lrt -Wall -Wextra")

file(GLOB_RECURSE SOURCE_FILES update/*.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
