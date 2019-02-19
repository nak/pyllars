include_directories(.
        /usr/local/include/python3.7m
        /opt/googletest/include)

include_directories(/opt/googletest/googletest/include /opt/googletest/googletest/
        ../src/resources
        ..)

add_executable(runPyllarsTest
        /opt/googletest/googletest/src/gtest_main.cc
        /opt/googletest/googletest/src/gtest-all.cc
        cpp/test_classes_and_pointers.cpp)

target_link_libraries(runPyllarsTest PUBLIC -L/usr/local/lib64 -lpython3.7m pthread pyllars ffi)
