cmake_minimum_required(VERSION 3.10)
project(Lab1_1)

# Установка стандарта C++
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Определение исходных файлов для каждого исполняемого файла
set(CREATOR_SOURCES
    src/creator/creator.cpp
)

set(MAIN_SOURCES
    src/main/main.cpp
)

# Добавление директории с заголовочными файлами
include_directories(${CMAKE_SOURCE_DIR}/src)

# Создание исполняемых файлов
add_executable(creator ${CREATOR_SOURCES})
add_executable(main ${MAIN_SOURCES})

# Установка выходного каталога для исполняемых файлов
set_target_properties(creator main
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Установка main.cpp как стартовой точки проекта
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT main)