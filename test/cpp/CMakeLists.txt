
include_directories(
        ${PYTHON_INCLUDE}
        ${GTEST_INCLUDE}
        ../../src/cpp
        ../../src/python/pyllars/resources/include
        ../../src/python/pyllars/resources/include/pyllars
        ../..)

add_library(setupBasicClassLib SHARED setup_basic_class.cpp setup_basic_class-builder.cpp setup_basic_class_BasicClass.cpp setup_basic_class_BasicClass2.cpp)
target_link_libraries(setupBasicClassLib pyllars gtest gtest_main)
add_library(setupInheritanceLib SHARED setup_inheritance_test.cpp )
target_link_libraries(setupInheritanceLib pyllars gtest gtest_main setupBasicClassLib)
add_library(setupBitFieldsLib SHARED setup_bitfields.cpp )
target_link_libraries(setupBitFieldsLib pyllars gtest gtest_main)

add_library(setupAnonEnumsLib SHARED setup_anon_enums.cpp )
target_link_libraries(setupAnonEnumsLib pyllars gtest gtest_main)
add_library(setupFunctions SHARED setup_functions.cpp )
target_link_libraries(setupFunctions pyllars gtest gtest_main)
add_library(setupGlobals SHARED setup_globals.cpp globals.cpp)
target_link_libraries(setupGlobals pyllars gtest gtest_main)
add_library(setupTemplateClasses SHARED setup_template_class.cpp )
target_link_libraries(setupTemplateClasses pyllars gtest gtest_main)



if (MSVC)

    add_executable(runAllPyllarsTest
            test_anon_enums.cpp
            setup_anon_enums.hpp
            test_basic_classes.cpp
            test_inheritance.cpp
            test_bit_fields.cpp
            test_functions.cpp
            function_defns.hpp globals.h globals.cpp)
    target_link_libraries(runAllPyllarsTest PUBLIC gtest_main
            setupBasicClassLib setupInheritanceLib setupBitFieldsLib setupAnonEnumsLib setupFunctions
            pyllars gtest_main)
else()

    add_executable(runAllPyllarsTest
            test_anon_enums.cpp
            setup_anon_enums.hpp
            test_conversions.cpp
            test_basic_classes.cpp
            test_inheritance.cpp
            test_bit_fields.cpp
            test_functions.cpp
            function_defns.hpp globals.h globals.cpp)
    target_link_libraries(runAllPyllarsTest PUBLIC -L/usr/local/lib64 ${Python3_LIBRARIES} gtest_main
        setupBasicClassLib setupInheritanceLib setupBitFieldsLib setupAnonEnumsLib setupFunctions
        pyllars pthread  ffi dl util gtest_main)
endif()

add_executable(runGlobalsTests
        test_globals.cpp
        globals.cpp
        )
if (MSVC)
    target_link_libraries(runGlobalsTests PUBLIC setupGlobals pyllars gtest_main)
else()
    target_link_libraries(runGlobalsTests PUBLIC setupGlobals pthread  -L/usr/local/lib64  ${Python3_LIBRARIES} pyllars ffi dl gtest_main)
endif()

add_executable(runBasicClassTests
        test_basic_classes.cpp)
if (MSVC)
    target_link_libraries(runBasicClassTests PUBLIC  gtest_main setupBasicClassLib  pyllars )
else()
    target_link_libraries(runBasicClassTests PUBLIC ${Python3_LIBRARIES} gtest_main
            setupBasicClassLib  pthread pyllars ffi dl util )
endif()

add_executable(runTemplateClassTests
       test_template_classes.cpp)
if (MSVC)
    target_link_libraries(runTemplateClassTests PUBLIC gtest_main setupTemplateClasses  pyllars)
else()
    target_link_libraries(runTemplateClassTests PUBLIC -L/usr/local/lib64 ${Python3_LIBRARIES} gtest_main
            setupTemplateClasses  pthread pyllars ffi dl util )
endif()

add_executable(runInheritanceTests test_inheritance.cpp)
if (MSVC)
    target_link_libraries(runInheritanceTests PUBLIC gtest_main setupInheritanceLib setupBasicClassLib pyllars)

else()
    target_link_libraries(runInheritanceTests PUBLIC -L/usr/local/lib64  ${Python3_LIBRARIES} gtest_main
            setupInheritanceLib setupBasicClassLib
            pthread pyllars ffi dl util
           )
endif()
add_executable(runBitFieldTests test_bit_fields.cpp)
if (MSVC)
    target_link_libraries(runBitFieldTests PUBLIC -L/usr/local/lib64 ${Python3_LIBRARIES}  setupBitFieldsLib pyllars)
else()
    target_link_libraries(runBitFieldTests PUBLIC -L/usr/local/lib64 ${Python3_LIBRARIES} pthread setupBitFieldsLib
       pyllars ffi dl util)
endif()

add_executable(runAnonEmumTests
        test_anon_enums.cpp
        )
if (MSVC)
    target_link_libraries(runAnonEmumTests PUBLIC setupAnonEnumsLib  pyllars)
else()
    target_link_libraries(runAnonEmumTests PUBLIC -L/usr/local/lib64   ${Python3_LIBRARIES}
            setupAnonEnumsLib pthread pyllars ffi dl util)
endif()

add_executable(runFunctionTests test_functions.cpp)

if (MSVC)
    target_link_libraries(runFunctionTests PUBLIC setupFunctions pyllars gtest_main)
else()
    target_link_libraries(runFunctionTests PUBLIC -L/usr/local/lib64   ${Python3_LIBRARIES}
        setupFunctions pthread pyllars ffi dl util)
endif()

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
