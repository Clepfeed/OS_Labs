cmake_minimum_required(VERSION 3.10)
project(named_pipes_project)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Добавляем директорию include
include_directories(include)

# Сервер
add_executable(Server src/server.cpp)

# Клиент
add_executable(Client src/client.cpp)