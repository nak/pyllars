include_directories(.
        /usr/local/include/python3.7m
        /opt/googletest/include)

include_directories(/opt/googletest/googletest/include /opt/googletest/googletest/
        ../src/resources
        ..)

add_library(setupBasicClassLib SHARED cpp/setup_basic_class.cpp )
add_library(setupInheritanceLib SHARED cpp/setup_inheritance_test.cpp )
add_library(setupBitFieldsLib SHARED cpp/setup_bitfields.cpp )
add_library(setupAnonEnumsLib SHARED cpp/setup_anon_enums.cpp )

add_executable(runAllPyllarsTest
        /opt/googletest/googletest/src/gtest_main.cc
        /opt/googletest/googletest/src/gtest-all.cc
        cpp/test_anon_enums.cpp
        cpp/test_conversions.cpp
        cpp/class_test_defns.h
        cpp/anon_enum_defns.h
        cpp/setup.h
        cpp/setup_basic_class.h
        cpp/setup_anon_enums.hpp
        cpp/setup_bitfields.h
        cpp/setup_inheritance_test.h
        cpp/test_basic_classes.cpp
        cpp/test_inheritance.cpp
        cpp/test_bit_fields.cpp
        cpp/test_initializer.cpp)

target_link_libraries(runAllPyllarsTest PUBLIC -L/usr/local/lib64 -lpython3.7m
        setupBasicClassLib setupInheritanceLib setupBitFieldsLib setupAnonEnumsLib
        pyllars pthread  ffi dl util)

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

add_executable(runBasicClassTests
        /opt/googletest/googletest/src/gtest_main.cc
        /opt/googletest/googletest/src/gtest-all.cc
        cpp/test_basic_classes.cpp)
target_link_libraries(runBasicClassTests PUBLIC -L/usr/local/lib64 -lpython3.7m
        setupBasicClassLib  pthread pyllars ffi dl util )


add_executable(runInheritanceTests
        /opt/googletest/googletest/src/gtest_main.cc
        /opt/googletest/googletest/src/gtest-all.cc
        cpp/test_inheritance.cpp
        )
target_link_libraries(runInheritanceTests PUBLIC -L/usr/local/lib64 -lpython3.7m
        setpInheritanceLib setupBasicClassLib
        pthread pyllars ffi dl util
       )

add_executable(runBitFieldTests
        /opt/googletest/googletest/src/gtest_main.cc
        /opt/googletest/googletest/src/gtest-all.cc
        cpp/test_bit_fields.cpp)
target_link_libraries(runBitFieldTests PUBLIC -L/usr/local/lib64 -lpython3.7m pthread setupBitFieldsLib
        pyllars ffi dl util)

add_executable(runInitializerTests
        /opt/googletest/googletest/src/gtest_main.cc
        /opt/googletest/googletest/src/gtest-all.cc
        cpp/test_initializer.cpp)
target_link_libraries(runInitializerTests PUBLIC -L/usr/local/lib64 -lpython3.7m pthread pyllars ffi dl util)

add_executable(runAnonEmumTests
        /opt/googletest/googletest/src/gtest_main.cc
        /opt/googletest/googletest/src/gtest-all.cc
        cpp/test_anon_enums.cpp
        )
target_link_libraries(runAnonEmumTests PUBLIC -L/usr/local/lib64 -lpython3.7m
        setupAnonEnumsLib pthread pyllars ffi dl util)
