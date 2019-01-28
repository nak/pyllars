cmake_minimum_required(VERSION 3.13)
project(pyllars)

set(CMAKE_CXX_STANDARD 14)

include_directories(.
        /opt/Python3.7/include/python3.7m)

add_library(pyllars
        pyllars.cpp
        pyllars.hpp
        pyllars_callbacks.hpp
        pyllars_callbacks.impl
        pyllars_classmembersemantics.hpp
        pyllars_classmembersemantics.impl
        pyllars_classmethodsemantics.hpp
        pyllars_classmethodsemantics.impl
        pyllars_classwrapper.hpp
        pyllars_classwrapper.impl
        pyllars_const.cpp
        pyllars_const.hpp
        pyllars_conversions.hpp
        pyllars_conversions.impl
        pyllars_defns.hpp
        pyllars_defns.impl
        pyllars_function_wrapper.hpp
        pyllars_globalmembersemantics.hpp
        pyllars_globalmembersemantics.impl
        pyllars_membersemantics.hpp
        pyllars_membersemantics.impl
        pyllars_methodcallsemantics.hpp
        pyllars_methodcallsemantics.impl
        pyllars_object_lifecycle.hpp
        pyllars_object_lifecycle.impl
        pyllars_pointer.hpp
        pyllars_templates.hpp
        pyllars_templates.impl
        pyllars_utils.hpp
        pyllars_utils.impl
        pyllars_varargs.hpp
        pyllars_varargs.impl)
