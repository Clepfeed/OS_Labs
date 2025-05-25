cmake_minimum_required(VERSION 3.10)
project(ProcessSync)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Создаем библиотеку с общими функциями
add_library(message_utils
    src/message_utils.cpp
    include/message_utils.h
    include/header.h
)

target_include_directories(message_utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Создаем исполняемые файлы
add_executable(receiver src/receiver.cpp)
add_executable(sender src/sender.cpp)

# Связываем с библиотекой
target_link_libraries(receiver message_utils)
target_link_libraries(sender message_utils)

# Добавляем тесты
enable_testing()

add_executable(message_tests tests/message_tests.cpp)
target_link_libraries(message_tests
    gtest
    gtest_main
    message_utils
)

add_test(NAME MessageTests COMMAND message_tests)

# Пути для включаемых файлов
target_include_directories(receiver PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(sender PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(message_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)