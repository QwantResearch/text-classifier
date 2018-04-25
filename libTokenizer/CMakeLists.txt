cmake_minimum_required(VERSION 3.0.0)
project(qtranslate)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE Release)
SET(CMAKE_VERBOSE_MAKEFILE ON)

MESSAGE( STATUS "CMAKE_CXX_FLAGS:         " ${CMAKE_CXX_FLAGS})
find_package(OpenMP)
set(CMAKE_CXX_FLAGS " -std=c++11 ${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
MESSAGE( STATUS "CMAKE_CXX_FLAGS:         " ${CMAKE_CXX_FLAGS})
FIND_PACKAGE(pybind11 REQUIRED)
find_package(Eigen3 3.3 REQUIRED)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(/usr/include/python3.5m/)
include_directories(${EIGEN3_INCLUDE_DIR})

FIND_LIBRARY(QNLP_LIB qnlp /usr/local/lib)
FIND_LIBRARY(FASTTEXT_LIB fasttext /usr/local/lib)
# TARGET_LINK_LIBRARIES(testOVAS ${QNLP_LIB})


SET(QTRANS_SRC
	src/qtranslate.cpp 
	src/BatchReader.cpp 
	src/BatchWriter.cpp
)

#pybind11_add_module(${PROJECT_NAME} ${QTRANS_SRC}) 
add_library(${PROJECT_NAME} SHARED ${QTRANS_SRC})
target_link_libraries(${PROJECT_NAME} ${QNLP_LIB} ${FASTTEXT_LIB} pybind11::module qnlp onmt fasttext)


install(TARGETS ${PROJECT_NAME}
  LIBRARY DESTINATION lib/
  ARCHIVE DESTINATION lib/
  COMPONENT python
)



