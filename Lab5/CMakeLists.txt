cmake_minimum_required(VERSION 3.10)
project(named_pipes_project)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ��������� ���������� include
include_directories(include)

# ������
add_executable(Server src/server.cpp)

# ������
add_executable(Client src/client.cpp)