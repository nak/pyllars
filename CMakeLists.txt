cmake_minimum_required(VERSION 3.10)
project(pyllars)
set(CMAKE_VERBOSE_MAKEFILE ON)
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
set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
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
    set (CMAKE_CXX_FLAGS "/IGNORE:C4251" ${CMAKE_CXX_FLAGS})
else()
    #set(GCC_COVERAGE_COMPILE_FLAGS "-fprofile-arcs -ftest-coverage --coverage -O0 -fno-inline")
    set(GCC_COVERAGE_COMPILE_FLAGS "-O0")
    set(GCC_COVERAGE_LINK_FLAGS    "--coverage -lgcov")
    SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS} ")
    SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")
endif()
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 17)

if (MSVC)
else()
endif()
find_package (Python3 COMPONENTS Development)

if (MSVC)
    SET (LIBFFI_ROOT "libffi")
    include_directories(${CMAKE_BINARY_DIR}/../${LIBFFI_ROOT}/include)
    link_directories(${CMAKE_BINARY_DIR}/../${LIBFFI_ROOT}/bin)
    set (LIBFFI_LIB ${CMAKE_BINARY_DIR}/../${LIBFFI_ROOT}/bin/libffi-6.dll)
    message("#################LIBFFI: ${LIBFFI_LIB}")
else()
    string(REPLACE "python3.7m" "python3.7dm" Python3dm_INCLUDE_DIRS "${Python3_INCLUDE_DIRS}")
    string(REPLACE "python3.7m" "python3.7dm" Python3dm_LIBRARIES "${Python3_LIBRARIES}")
    if (EXISTS ${Python3dm_INCLUDE_DIRS})
        message("#### USING DM Python build: ${Python3dm_INCLUDE_DIRS}")
        set (Python3_INCLUDE_DIRS ${Python3dm_INCLUDE_DIRS})
        set (Python3_LIBRARIES ${Python3dm_LIBRARIES})
    endif()
endif()
include_directories(${Python3_INCLUDE_DIRS})
link_directories(${Python3_LIBRARY_DIRS})
message("##################### Python: ${Python3_FOUND} ${Python3_INCLUDE_DIRS} ${Python_3LIBRARY_DIRS} ${Python3_LIBRARIES}")
add_subdirectory(./src)
add_subdirectory(./test)