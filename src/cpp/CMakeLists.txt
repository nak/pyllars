project(pyllars CXX)
include_directories(.
		${PYTHON_INCLUDE}
		../python/pyllars/resources/include
		../python/pyllars/resources/include/pyllars)
if (MSVC)
	set (CMAKE_CXX_FLAGS  ${CMAKE_CXX_FLAGS} )
	message("############### DLL defined")
endif()

add_library(pyllars SHARED
		pyllars/pyllars.cpp
		pyllars/pyllars_floating_point.cpp
		pyllars/pyllars_integer.impl.hpp
		pyllars/pyllars_integer_types.cpp
		pyllars/pyllars_integer_const.cpp
		pyllars/pyllars_integer_constvolatile.cpp
		pyllars/pyllars_integer_nonconst.cpp
		pyllars/pyllars_integer_nonconstvolatile.cpp
		pyllars/pyllars_templates.cpp
		pyllars/pyllars_integer_refs.cpp
		pyllars/pyllars_integer_refs_const.cpp
		pyllars/pyllars_integer_refs_volatile.cpp
		pyllars/pyllars_integer_refs_const_volatile.cpp
		pyllars/pyllars_integer_ptrs.cpp
		pyllars/pyllars_integer_ptrs_volatile.cpp
		pyllars/pyllars_integer_ptrs_const.cpp
		pyllars/pyllars_integer_ptrs_const_volatile.cpp
		pyllars/pyllars_floating_point_ptrs.cpp ../python/pyllars/resources/include/pyllars/internal/pyllars_base.hpp ../python/pyllars/resources/include/pyllars/internal/pyllars_base.impl.hpp ../python/pyllars/resources/include/pyllars/internal/pyllars_base-conversions.impl.hpp)


set_target_properties(pyllars PROPERTIES LIBRARY_OUTPUT_DIRECTORY ../python/pyllars/resources/lib)

if(MSVC)
else()
	target_link_libraries(pyllars PUBLIC -Wl,--no-undefined  -Wl,-rpath '$ORIGIN' ${Python3_LIBRARIES} -lffi)
endif()