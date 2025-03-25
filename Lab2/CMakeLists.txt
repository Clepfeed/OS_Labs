cmake_minimum_required (VERSION 3.8)

project ("Lab2")

# Enable testing
enable_testing()

# Download and build GoogleTest
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.zip
)
FetchContent_MakeAvailable(googletest)

# Define source files for C++98 version
set(MAIN98
    98/main.cpp
)

# Define source files for C++17 version
set(MAIN17
    17/main.cpp
)

# Create C++98 executables
add_executable(main98 ${MAIN98})
set_target_properties(main98 PROPERTIES
    CXX_STANDARD 98
    CXX_STANDARD_REQUIRED ON
)

add_executable(tests98 
    ${MAIN98}
    98/test/test.cpp
)
set_target_properties(tests98 PROPERTIES
    CXX_STANDARD 98
    CXX_STANDARD_REQUIRED ON
)

# Create C++17 executables
add_executable(main17 ${MAIN17})
set_target_properties(main17 PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

add_executable(tests17
    ${MAIN17}
    17/test/test.cpp
)
set_target_properties(tests17 PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# Link tests with GTest
target_compile_definitions(tests98 PRIVATE TESTING)
target_compile_definitions(tests17 PRIVATE TESTING)
target_link_libraries(tests98 PRIVATE gtest gtest_main)
target_link_libraries(tests17 PRIVATE gtest gtest_main)

# Add tests to CTest
include(GoogleTest)
gtest_discover_tests(tests98)
gtest_discover_tests(tests17)

# Set output directories
set_target_properties(main98 tests98 main17 tests17
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)