cmake_minimum_required(VERSION 3.3)
project(Pyllars)

set(CMAKE_VERBOSE_MAKEFILE  on)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++14  -Werror -Wall -std=c++11")

include_directories(/usr/include/python2.7)
include_directories(/usr/include)
include_directories(src)
link_directories(/usr/lib/x86_64-linux-gnu)

set(SOURCE_FILES

        /usr/include/python2.7/Python.h
        /usr/include/python2.7/object.h)

add_custom_target(build_globals_module ALL PYTHONPATH=../src pytest -s ./test_code_generation.py -k globals WORKING_DIRECTORY  ${PROJECT_SOURCE_DIR}/test)
