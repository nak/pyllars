cmake_minimum_required(VERSION 3.3)
project(Pyllars)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -std=c++11")

include_directories(/usr/include/python2.7)
include_directories(/usr/include)
include_directories(pyllars)
link_directories(/usr/lib/x86_64-linux-gnu)

set(SOURCE_FILES
        pyllars/test/test_basics.cpp
        pyllars/pyllars/pyllars_defns.hpp
        pyllars/pyllars/pyllars_utils.hpp
        pyllars/pyllars/pyllars_pointer.hpp
        pyllars/pyllars/pyllars_object_lifecycle.hpp
        pyllars/pyllars/pyllars_methodcallsemantics.hpp
        pyllars/pyllars/pyllars_globalmembersemantics.hpp
        pyllars/pyllars/pyllars_function_wrapper.hpp
        pyllars/pyllars/pyllars_conversions.hpp
        pyllars/pyllars/pyllars_constmethodcallsemantics.hpp
        pyllars/pyllars/pyllars_classwrapper.hpp
        pyllars/pyllars/pyllars_classmethodsemantics.hpp
        pyllars/pyllars/pyllars_callbacks.hpp

        /usr/include/python2.7/Python.h
        /usr/include/python2.7/object.h
        /usr/include/python2.7/tupleobject.h
        pyllars/pyllars/pyllars_callbacks.cpp pyllars/pyllars/pyllars_conversions.cpp pyllars/pyllars/pyllars_constmethodcallsemantics.cpp pyllars/pyllars/pyllars_globalmembersemantics.cpp pyllars/pyllars/pyllars_methodcallsemantics.cpp)
add_definitions(-DMAIN)
add_executable(Pyllars ${SOURCE_FILES})
target_link_libraries(Pyllars  python2.7 )

add_custom_target(build_module ALL python setup_basic_C.py build_ext WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/pyllars/test  DEPENDS ${PROJECT_SOURCE_DIR}/pyllars/test/to_convert/testbasic_C.hpp ${PROJECT_SOURCE_DIR}/pyllars/pyllars/xslt/cpp2pyllars.xslt)
