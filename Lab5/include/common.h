#pragma once

#include <windows.h>
#include <string>

// ��������� ����������
struct Employee {
    int num;        // ����������������� ����� ����������
    char name[10];  // ��� ����������
    double hours;   // ���������� ������������ �����
};

// ��������� ��� ������������ ������
const std::string PIPE_NAME = "\\\\.\\pipe\\employee_file_access";

// ���� ��������
enum RequestType {
    READ_REQUEST = 1,
    WRITE_REQUEST = 2,
    COMPLETE_REQUEST = 3
};

// ��������� �������
struct Request {
    RequestType type;
    int employeeId;
    Employee data;
};

// ��������� ������
struct Response {
    bool success;
    Employee data;
};