
include_directories(.
        /usr/local/include/python3.7m
        ../python/pyllars/resources/include
	../python/pyllars/resources/include/pyllars)

add_library(pyllars SHARED
        pyllars/pyllars.cpp
        pyllars/pyllars_templates.cpp
        pyllars/pyllars_templates.cpp)
set_target_properties(pyllars PROPERTIES LIBRARY_OUTPUT_DIRECTORY ../python/pyllars/resources/lib)
target_link_libraries(pyllars PUBLIC -Wl,-rpath '$ORIGIN')
