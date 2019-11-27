
include_directories(
        ${PYTHON_INCLUDE}
        ${GTEST_INCLUDE}
        ../../src/cpp
        ../../src/python/pyllars/resources/include
        ../../src/python/pyllars/resources/include/pyllars
        ../..)

add_library(setupBasicClassLib SHARED setup_basic_class.cpp setup_basic_class-builder.cpp)
add_library(setupInheritanceLib SHARED setup_inheritance_test.cpp )
add_library(setupBitFieldsLib SHARED setup_bitfields.cpp )
add_library(setupAnonEnumsLib SHARED setup_anon_enums.cpp )
add_library(setupFunctions SHARED setup_functions.cpp )
add_library(setupGlobals SHARED setup_globals.cpp )
add_library(setupTemplateClasses SHARED setup_template_class.cpp )


add_executable(runAllPyllarsTest
        test_anon_enums.cpp
        setup_anon_enums.hpp
        test_conversions.cpp
        test_basic_classes.cpp
        test_inheritance.cpp
        test_bit_fields.cpp
        test_functions.cpp
        function_defns.hpp globals.h globals.cpp)


target_link_libraries(runAllPyllarsTest PUBLIC -L/usr/local/lib64 ${PYTHON_LD} gtest_main
        setupBasicClassLib setupInheritanceLib setupBitFieldsLib setupAnonEnumsLib setupFunctions
        pyllars pthread  ffi dl util gtest_main)

add_executable(runGlobalsTests
        test_globals.cpp
        globals.cpp
        )
target_link_libraries(runGlobalsTests PUBLIC setupGlobals pthread  -L/usr/local/lib64  ${PYTHON_LD} pyllars ffi dl gtest_main)

add_executable(runBasicClassTests
        test_basic_classes.cpp)
target_link_libraries(runBasicClassTests PUBLIC -L/usr/local/lib64 ${PYTHON_LD} gtest_main
        setupBasicClassLib  pthread pyllars ffi dl util )


add_executable(runTemplateClassTests
        test_template_classes.cpp)
target_link_libraries(runTemplateClassTests PUBLIC -L/usr/local/lib64 ${PYTHON_LD} gtest_main
        setupTemplateClasses  pthread pyllars ffi dl util )


add_executable(runInheritanceTests
        test_inheritance.cpp
        )
target_link_libraries(runInheritanceTests PUBLIC -L/usr/local/lib64  ${PYTHON_LD} gtest_main
        setupInheritanceLib setupBasicClassLib
        pthread pyllars ffi dl util
       )

add_executable(runBitFieldTests
        test_bit_fields.cpp)
target_link_libraries(runBitFieldTests PUBLIC -L/usr/local/lib64 ${PYTHON_LD} pthread setupBitFieldsLib
        pyllars ffi dl util)

add_executable(runAnonEmumTests
        test_anon_enums.cpp
        )
target_link_libraries(runAnonEmumTests PUBLIC -L/usr/local/lib64   ${PYTHON_LD}
        setupAnonEnumsLib pthread pyllars ffi dl util)

add_executable(runFunctionTests
        test_functions.cpp
        )
target_link_libraries(runFunctionTests PUBLIC -L/usr/local/lib64   ${PYTHON_LD}
        setupFunctions pthread pyllars ffi dl util)

add_custom_target(testCoverage
        DEPENDS runAllPyllarsTest
        COMMAND ./runAllPyllarsTest && mkdir -p res && lcov -o res/testcov.out -c -d . --exclude \"test_*\" && genhtml -o res res/testcov.out && google-chrome --new-window ./res/pyllars/index.html
        WORKING_DIRECTORY .
        COMMENT Generated test coverage)

add_custom_target(testCoverageContainment
        DEPENDS runContainmentTests
        COMMAND ./runContainmentTests && mkdir -p res && lcov -o res/testcov.out -c -d . --exclude \"test_*\" && genhtml -o res res/testcov.out && google-chrome --new-window ./res/pyllars/index.html
        WORKING_DIRECTORY .
        COMMENT Generated test coverage)
