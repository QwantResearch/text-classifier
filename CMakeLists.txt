# Copyright 2019 Qwant Research. Licensed under the terms of the Apache 2.0 license. See LICENSE in the project root.

cmake_minimum_required(VERSION 3.5)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lpthread")
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

include_directories(${PROJECT_SOURCE_DIR}/include/ /usr/local/include/)
link_directories(/usr/local/lib/)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -std=c++11 -lpthread")

set(CLASS_EXECUTABLE_API katanoisi)

set(CLASS_SOURCE_API ${PROJECT_SOURCE_DIR}/src/katanoisi.cpp
                     ${PROJECT_SOURCE_DIR}/src/rest_server.cpp
                     ${PROJECT_SOURCE_DIR}/src/tokenizer.cpp
                     ${PROJECT_SOURCE_DIR}/src/classifier.cpp
                     ${PROJECT_SOURCE_DIR}/src/utils.cpp)

add_executable(${CLASS_EXECUTABLE_API} ${CLASS_SOURCE_API})

target_link_libraries(${CLASS_EXECUTABLE_API} pistache pthread qnlp fasttext yaml-cpp)

install(TARGETS ${CLASS_EXECUTABLE_API} DESTINATION bin)
