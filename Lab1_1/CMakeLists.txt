cmake_minimum_required(VERSION 3.10)
project(Lab1_1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CREATOR_SOURCES src/creator/creator.cpp)
set(REPORTER_SOURCES src/reporter/reporter.cpp)

set(MAIN_SOURCES src/main/main.cpp)

include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable(creator ${CREATOR_SOURCES})
add_executable(reporter ${REPORTER_SOURCES})
add_executable(main ${MAIN_SOURCES})

set_target_properties(creator reporter main
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT main)