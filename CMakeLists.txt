# Copyright 2019 Qwant Research. Licensed under the terms of the Apache 2.0 license. See LICENSE in the project root.
PROJECT(text-classifier)

cmake_minimum_required(VERSION 3.5)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lpthread")
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

### GRPC ####
if(NOT MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
  add_definitions(-D_WIN32_WINNT=0x600)
endif()

set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)
message(STATUS "Using protobuf ${protobuf_VERSION}")

set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)

# Find gRPC installation
# Looks for gRPCConfig.cmake file installed by gRPC's cmake installation.
find_package(gRPC CONFIG REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")

set(_GRPC_GRPCPP_UNSECURE gRPC::grpc++_unsecure)
set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)

# Proto file
get_filename_component(hw_proto "protos/grpc_classification.proto" ABSOLUTE)
get_filename_component(hw_proto_path "${hw_proto}" PATH)

# Generated sources
set(hw_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/grpc_classification.pb.cc")
set(hw_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/grpc_classification.pb.h")
set(hw_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/grpc_classification.grpc.pb.cc")
set(hw_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/grpc_classification.grpc.pb.h")
add_custom_command(
      OUTPUT "${hw_proto_srcs}" "${hw_proto_hdrs}" "${hw_grpc_srcs}" "${hw_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${hw_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${hw_proto}"
      DEPENDS "${hw_proto}")

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")
### END GRPC ###

include_directories(${PROJECT_SOURCE_DIR}/include/ /usr/local/include/ /usr/)

link_directories(/usr/local/lib/ /usr/lib/)

set(CLASS_EXECUTABLE_API text-classifier)

set(CLASS_SOURCE_API ${PROJECT_SOURCE_DIR}/src/text-classifier.cpp
                     ${PROJECT_SOURCE_DIR}/src/rest_server.cpp
                     ${PROJECT_SOURCE_DIR}/src/grpc_server.cpp
                     ${PROJECT_SOURCE_DIR}/src/abstract_server.cpp
                     ${PROJECT_SOURCE_DIR}/src/tokenizer.cpp
                     ${PROJECT_SOURCE_DIR}/src/classifier.cpp
                     ${PROJECT_SOURCE_DIR}/src/utils.cpp
                     ${PROJECT_SOURCE_DIR}/src/classifier_controller.cpp
                     ${PROJECT_SOURCE_DIR}/src/grpc_route_classify_impl.cpp
                     ${CMAKE_CURRENT_BINARY_DIR}/grpc_classification.grpc.pb.cc
                     ${CMAKE_CURRENT_BINARY_DIR}/grpc_classification.pb.cc
                     ${CMAKE_CURRENT_BINARY_DIR}/grpc_classification.grpc.pb.h
                     ${CMAKE_CURRENT_BINARY_DIR}/grpc_classification.pb.h
)




add_executable(${CLASS_EXECUTABLE_API} ${CLASS_SOURCE_API})

set(LIBS
    ${_GRPC_GRPCPP_UNSECURE}
    ${_PROTOBUF_LIBPROTOBUF}
    fasttext
    qnlp
    pistache
    pthread
    yaml-cpp
)

target_link_libraries(${CLASS_EXECUTABLE_API} ${LIBS})

### Boost Tests ####

enable_testing()

find_package(Boost COMPONENTS unit_test_framework REQUIRED)

add_executable(tests
  tests/tests.cpp
  tests/tests_classifier.cpp
  tests/tests_tokenizer.cpp
  tests/tests_classifier_controller.cpp
)

add_library(text-classifier-project ${CLASS_SOURCE_API})

target_link_libraries(text-classifier-project ${LIBS})
target_link_libraries(tests text-classifier-project ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
# indicates the include paths
target_include_directories(tests PRIVATE ${Boost_INCLUDE_DIRS})
# indicates the shared library variant
target_compile_definitions(tests PRIVATE "BOOST_TEST_DYN_LINK=1")

# declares a test with our executable
add_test(NAME tests COMMAND tests)


### END Boost Tests ####

install(TARGETS ${CLASS_EXECUTABLE_API} tests DESTINATION bin)