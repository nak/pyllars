

include_directories(.
        /usr/local/include/python3.7m
        )

add_library(pyllars SHARED
        pyllars/pyllars.cpp
        pyllars/pyllars_templates.cpp
        pyllars/pyllars_templates.cpp)

target_link_libraries(pyllars PUBLIC -Wl,-rpath '$ORIGIN')
