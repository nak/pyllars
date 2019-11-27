cmake_minimum_required(VERSION 3.10)
project(pyllars)

configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
    message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
    message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
        ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
        EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
    include_directories("${gtest_SOURCE_DIR}/include")


if(MSVC)
else()
    set(GCC_COVERAGE_COMPILE_FLAGS "-fprofile-arcs -ftest-coverage --coverage -O0 -fno-inline")
    set(GCC_COVERAGE_LINK_FLAGS    "--coverage -lgcov")
    SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
    SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")
endif()
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 17)
find_package (Python COMPONENTS Development)

if (EXISTS "/usr/local/include/python3.7dm")
    set (PYTHON_LINK_DIR "/usr/local/lib64")
	set (PYTHON_LD "-lpython3.7dm")
    set (PYTHON_INCLUDE "/usr/local/include/python3.7dm")
elseif(EXISTS "C:\\Users\\johnj\\venv37")
    set(PYTHON_INCLUDE "C:\\Users\\johnj\\AppData\\Local\\Programs\\Python\\Python37\\include")
    set(PYTHON_LINK_DIR "C:\\Users\\johnj\\AppData\\Local\\Programs\\Python\\Python37\\libs")
else()
    set (PYTHON_LD "-lpython3.6m")
    set (PYTHON_INCLUDE "/usr/include/python3.6m")
endif()
if (MSVC)
    SET (LIBFFI_ROOT "libffi")
    include_directories(${CMAKE_BINARY_DIR}/../${LIBFFI_ROOT}/include)
    link_directories(${CMAKE_BINARY_DIR}/../${LIBFFI_ROOT}/bin)
    set (LIBFFI_LIB ${CMAKE_BINARY_DIR}/../${LIBFFI_ROOT}/bin/libffi-6.dll)
    message("#################LIBFFI: ${LIBFFI_LIB}")
endif()
#set (PYTHON_INCLUDE ${PYTHON_INCLUDE})
#set (PYTHON_LD "${PYTHON_LINK_DIR}}")
include_directories(${Python_INCLUDE_DIRS})
link_directories(${Python_LIBRARY_DIRS})
message("##################### ${PYTHON_FOUND} ${Python_INCLUDE_DIRS} ${Python_LIBRARY_DIRS}")
add_subdirectory(./src)
add_subdirectory(./test)
