project(pyllars CXX)
include_directories(.
		${PYTHON_INCLUDE}
		../python/pyllars/resources/include
		../python/pyllars/resources/include/pyllars)


add_library(pyllars SHARED
pyllars/pyllars.cpp
pyllars/pyllars_integer.cpp
pyllars/pyllars_templates.cpp
pyllars/pyllars_floating_point.cpp
pyllars/pyllars_integer.cpp ../python/pyllars/resources/include/pyllars/pyllars_namespacewrapper.hpp ../python/pyllars/resources/include/pyllars/pyllars_class.hpp ../python/pyllars/resources/include/pyllars/pyllars_classmethod.hpp pyllars/pyllars_class.cpp ../python/pyllars/resources/include/pyllars/internal/pyllars_classwrapper-methods.impl.hpp ../python/pyllars/resources/include/pyllars/internal/pyllars_funttraits.hpp)


FILE(GLOB pcheaders
		../python/pyllars/resources/include/pyllars/*hpp
		../python/pyllars/resources/include/pyllars/internal/*hpp
		)
target_precompile_headers(pyllars
		PUBLIC
		${pcheaders})

set_target_properties(pyllars PROPERTIES LIBRARY_OUTPUT_DIRECTORY ../python/pyllars/resources/lib)
target_link_libraries(pyllars PUBLIC -Wl,-rpath '$ORIGIN' ${PYTHON_LD})
