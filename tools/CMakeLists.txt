cmake_minimum_required(VERSION 3.0)

FIND_PACKAGE(PkgConfig REQUIRED)
pkg_check_modules(OPENSSL REQUIRED openssl)

project(fsw_update VERSION 1.0.1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -g -pthread -lrt -Wall -Wextra")

add_executable(fsw_update update/main.cpp)


project(wip2pepu)

include_directories(
    ${OPENSSL_INCLUDE_DIRS}
)

set(LIBRARIES
    ${OPENSSL_LDFLAGS}
)

add_executable(wip2pepu security/main.cpp)

target_link_libraries(wip2pepu ${LIBRARIES})

