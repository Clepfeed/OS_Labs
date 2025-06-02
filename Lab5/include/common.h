#pragma once

#include <windows.h>
#include <string>

// Структура сотрудника
struct Employee {
    int num;        // идентификационный номер сотрудника
    char name[10];  // имя сотрудника
    double hours;   // количество отработанных часов
};

// Константы для именованного канала
const std::string PIPE_NAME = "\\\\.\\pipe\\employee_file_access";

// Типы запросов
enum RequestType {
    READ_REQUEST = 1,
    WRITE_REQUEST = 2,
    COMPLETE_REQUEST = 3
};

// Структура запроса
struct Request {
    RequestType type;
    int employeeId;
    Employee data;
};

// Структура ответа
struct Response {
    bool success;
    Employee data;
};