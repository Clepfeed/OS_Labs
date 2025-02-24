# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Lab1")

# Устанавливаем стандарт C++
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Настраиваем пути для выходных файлов
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)

# Добавляем заголовочные файлы
include_directories(${PROJECT_SOURCE_DIR}/include)

# Добавляем исполняемые файлы
add_executable(Main src/main/main.cpp "include/header.h")
add_executable(Creator src/creator/creator.cpp "include/header.h")
add_executable(Reporter src/reporter/reporter.cpp "include/header.h")

# Копируем зависимые исполняемые файлы в директорию с Main
add_custom_command(TARGET Main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy 
        $<TARGET_FILE:Creator> $<TARGET_FILE_DIR:Main>
    COMMAND ${CMAKE_COMMAND} -E copy 
        $<TARGET_FILE:Reporter> $<TARGET_FILE_DIR:Main>
)

# Устанавливаем Main как стартовый проект
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
    PROPERTY VS_STARTUP_PROJECT Main)


# Include sub-projects.
add_subdirectory ("Lab1")
