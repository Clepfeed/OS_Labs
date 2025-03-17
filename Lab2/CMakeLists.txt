cmake_minimum_required (VERSION 3.8)

project ("Lab2")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(MAIN_98 98/main.cpp)

include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable(main98 ${MAIN_98})

set_target_properties(main98
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)