cmake_minimum_required(VERSION 3.10)
project(ProcessSync)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(receiver src/receiver.cpp)
add_executable(sender src/sender.cpp)

target_include_directories(receiver PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(sender PRIVATE ${CMAKE_SOURCE_DIR}/include)