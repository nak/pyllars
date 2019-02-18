

include_directories(.
        /usr/local/include/python3.7m
        )

add_library(pyllars SHARED
        pyllars/pyllars.cpp
        pyllars/pyllars_templates.cpp
        pyllars/pyllars_callbacks.impl
        pyllars/pyllars_classmembersemantics.impl
        pyllars/pyllars_classmethodsemantics.impl
        pyllars/pyllars_classwrapper.impl
        pyllars/pyllars_conversions.impl
        pyllars/pyllars_defns.impl
        pyllars/pyllars_globalmembersemantics.impl
        pyllars/pyllars_membersemantics.impl
        pyllars/pyllars_methodcallsemantics.impl
        pyllars/pyllars_object_lifecycle.impl
        pyllars/pyllars_templates.cpp
        pyllars/pyllars_utils.impl
        pyllars/pyllars_varargs.impl)

target_link_libraries(pyllars PUBLIC -Wl,-rpath '$ORIGIN')
