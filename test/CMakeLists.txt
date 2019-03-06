include_directories(.
        /usr/local/include/python3.7m
        /opt/googletest/include)

include_directories(/opt/googletest/googletest/include /opt/googletest/googletest/
        ../src/resources
        ..)

add_executable(runPyllarsTest
        /opt/googletest/googletest/src/gtest_main.cc
        /opt/googletest/googletest/src/gtest-all.cc
        cpp/test_classes_and_pointers.cpp
        cpp/test_conversions.cpp)

target_link_libraries(runPyllarsTest PUBLIC -L/usr/local/lib64 -lpython3.7m pthread pyllars ffi dl util)

add_custom_target(testCoverage ALL
        DEPENDS runPyllarsTest
        COMMAND ./runPyllarsTest && mkdir -p res && lcov -o res/testcov.out -c -d . && genhtml -o res res/testcov.out && google-chrome --new-window ./res/pyllars/index.html
        WORKING_DIRECTORY .
        COMMENT Generated test coverage)

add_executable(runContainmentTests
        /opt/googletest/googletest/src/gtest_main.cc
        /opt/googletest/googletest/src/gtest-all.cc
        cpp/test_containment.cpp
        )
target_link_libraries(runContainmentTests PUBLIC pthread)

add_custom_target(testCoverageContainment ALL
        DEPENDS runContainmentTests
        COMMAND ./runContainmentTests && mkdir -p res && lcov -o res/testcov.out -c -d . && genhtml -o res res/testcov.out && google-chrome --new-window ./res/pyllars/index.html
        WORKING_DIRECTORY .
        COMMENT Generated test coverage)
