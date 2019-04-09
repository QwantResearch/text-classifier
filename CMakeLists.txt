########################################################################## #
#
#MIT License
#
#Copyright(c) 2019 Qwant Research
#
#Permission is hereby granted, free of charge, to any person obtaining a copy
#of this software and associated documentation files(the "Software"), to deal
#in the Software without restriction, including without limitation the rights
#to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
#copies of the Software, and to permit persons to whom the Software is
#furnished to do so, subject to the following conditions:
#
#The above copyright notice and this permission notice shall be included in
#all copies or substantial portions of the Software.
#
#THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
#AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
#SOFTWARE.
#
#Authors:
#Christophe Servan < c.servan @qwantresearch.com>
#Noel Martin < n.martin @qwantresearch.com>
#
    ########################################################################## #cmake_minimum_required(
        VERSION 3.5)

        set(CMAKE_EXE_LINKER_FLAGS
            "${CMAKE_EXE_LINKER_FLAGS} -lpthread -fpermissive ")
            set(CMAKE_VERBOSE_MAKEFILE ON) set(CMAKE_BUILD_TYPE RelWithDebInfo)

                include_directories(${PROJECT_SOURCE_DIR} / include / usr /
                                    local /
                                    include /) link_directories(/ usr / local /
                                                                lib /)

                    set(CMAKE_CXX_FLAGS
                        "${CMAKE_CXX_FLAGS} -O2 -std=c++11 -lpthread "
                        "-fpermissive ")

                        set(CLASS_EXECUTABLE_API qclass_server)
                            set(CLASS_SOURCE_API ${PROJECT_SOURCE_DIR} / src /
                                qclass_server.cpp ${PROJECT_SOURCE_DIR} / src /
                                qtokenizer.cpp ${PROJECT_SOURCE_DIR} / src /
                                qclassifier.cpp ${PROJECT_SOURCE_DIR} / src /
                                main_server.cpp) add_executable(${
                                CLASS_EXECUTABLE_API} ${CLASS_SOURCE_API})
                                target_link_libraries(${
                                    CLASS_EXECUTABLE_API} pistache pthread qnlp
                                                          fasttext)
                                    install(TARGETS ${
                                        CLASS_EXECUTABLE_API} DESTINATION bin)
