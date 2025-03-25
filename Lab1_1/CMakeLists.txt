cmake_minimum_required (VERSION 3.10)
project(Lab1_1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Основные исходные файлы (без функций main)
set(CREATOR_SOURCES src/creator/creator.cpp)
set(REPORTER_SOURCES src/reporter/reporter.cpp)
set(MAIN_SOURCES src/main/main.cpp)

include_directories(${CMAKE_SOURCE_DIR}/src)

# Основные исполняемые файлы
add_executable(creator ${CREATOR_SOURCES})
add_executable(reporter ${REPORTER_SOURCES})
add_executable(main ${MAIN_SOURCES})

set_target_properties(creator reporter main
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT main)

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(creator_test 
    tests/creator_test.cpp
    ${CMAKE_SOURCE_DIR}/src/creator/creator.cpp
)
target_compile_definitions(creator_test PRIVATE TESTING=1)
target_link_libraries(creator_test gtest_main gmock_main)
target_include_directories(creator_test PRIVATE ${CMAKE_SOURCE_DIR}/src/creator) ###

add_executable(reporter_test 
    tests/reporter_test.cpp
    ${CMAKE_SOURCE_DIR}/src/reporter/reporter.cpp
)
target_compile_definitions(reporter_test PRIVATE TESTING=1)
target_link_libraries(reporter_test gtest_main gmock_main)
target_include_directories(reporter_test PRIVATE ${CMAKE_SOURCE_DIR}/src/reporter)


add_executable(main_test 
    tests/main_test.cpp
    ${CMAKE_SOURCE_DIR}/src/main/main.cpp
)
target_compile_definitions(main_test PRIVATE TESTING=1)
target_link_libraries(main_test gtest_main gmock_main)
target_include_directories(main_test PRIVATE ${CMAKE_SOURCE_DIR}/src/main)

enable_testing()
include(GoogleTest)
gtest_discover_tests(creator_test)
gtest_discover_tests(reporter_test)
gtest_discover_tests(main_test)