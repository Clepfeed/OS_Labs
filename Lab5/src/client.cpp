#include <iostream>
#include <windows.h>
#include <string>
#include "../include/common.h"

HANDLE connectToServer() {
    HANDLE hPipe;

    // �������� ������������ � ������������ ������
    while (true) {
        hPipe = CreateFile(
            PIPE_NAME.c_str(),      // ��� ������
            GENERIC_READ |          // ����� �������
            GENERIC_WRITE,
            0,                      // ����� ����������� �������������
            NULL,                   // �������� ������������
            OPEN_EXISTING,          // �������� ��� ��������
            0,                      // �������� � �����
            NULL);                  // ������ �����

        if (hPipe != INVALID_HANDLE_VALUE) {
            break;
        }

        if (GetLastError() != ERROR_PIPE_BUSY) {
            std::cerr << "������ ����������� � �������: " << GetLastError() << std::endl;
            return INVALID_HANDLE_VALUE;
        }

        if (!WaitNamedPipe(PIPE_NAME.c_str(), 5000)) {
            std::cerr << "������� �������� ������\n";
            return INVALID_HANDLE_VALUE;
        }
    }

    DWORD dwMode = PIPE_READMODE_MESSAGE;
    if (!SetNamedPipeHandleState(hPipe, &dwMode, NULL, NULL)) {
        std::cerr << "������ ��������� ������ ������\n";
        CloseHandle(hPipe);
        return INVALID_HANDLE_VALUE;
    }

    return hPipe;
}

void displayEmployee(const Employee& emp) {
    std::cout << "ID: " << emp.num << "\n";
    std::cout << "���: " << emp.name << "\n";
    std::cout << "����: " << emp.hours << "\n";
}

// ������� ��� ������ ������
void readRecord(HANDLE hPipe) {
    DWORD bytesWritten, bytesRead;
    Request request;
    Response response;

    request.type = READ_REQUEST;

    std::cout << "������� ID ����������: ";
    std::cin >> request.employeeId;

    // ���������� ������ �� ������
    if (!WriteFile(hPipe, &request, sizeof(Request), &bytesWritten, NULL)) {
        std::cerr << "������ �������� �������\n";
        return;
    }

    // �������� �����
    if (!ReadFile(hPipe, &response, sizeof(Response), &bytesRead, NULL)) {
        std::cerr << "������ ��������� ������\n";
        return;
    }

    if (response.success) {
        std::cout << "\n������ ����������:\n";
        displayEmployee(response.data);
    }
    else {
        std::cout << "��������� � ID " << request.employeeId << " �� ������\n";
    }

    std::cout << "������� Enter ��� ���������� ������...";
    std::cin.ignore();
    std::cin.get();

    // �������� ������� � ���������� ������
    request.type = COMPLETE_REQUEST;
    WriteFile(hPipe, &request, sizeof(Request), &bytesWritten, NULL);
}

// ������� ��� ����������� ������
void modifyRecord(HANDLE hPipe) {
    DWORD bytesWritten, bytesRead;
    Request request;
    Response response;

    request.type = WRITE_REQUEST;

    std::cout << "������� ID ���������� ��� �����������: ";
    std::cin >> request.employeeId;

    // ���������� ������ �� ������
    if (!WriteFile(hPipe, &request, sizeof(Request), &bytesWritten, NULL)) {
        std::cerr << "������ �������� �������\n";
        return;
    }

    // �������� �����
    if (!ReadFile(hPipe, &response, sizeof(Response), &bytesRead, NULL)) {
        std::cerr << "������ ��������� ������\n";
        return;
    }

    if (!response.success) {
        std::cout << "��������� � ID " << request.employeeId << " �� ������ ��� ������ �������������\n";
        return;
    }

    std::cout << "\n������� ������ ����������:\n";
    displayEmployee(response.data);

    // ����������� ����� ��������
    request.data = response.data;
    std::cout << "\n������� ����� �������� (Enter - �������� ��� ���������):\n";

    std::cout << "��� [" << request.data.name << "]: ";
    std::cin.ignore();
    std::string newName;
    std::getline(std::cin, newName);
    if (!newName.empty()) {
        strncpy_s(request.data.name, newName.c_str(), sizeof(request.data.name) - 1);
    }

    std::cout << "���� [" << request.data.hours << "]: ";
    std::string hoursStr;
    std::getline(std::cin, hoursStr);
    if (!hoursStr.empty()) {
        request.data.hours = std::stod(hoursStr);
    }

    // ���������� ���������� ������
    if (!WriteFile(hPipe, &request, sizeof(Request), &bytesWritten, NULL)) {
        std::cerr << "������ �������� ���������������� ������\n";
        return;
    }

    std::cout << "\n������ ������� ���������\n";
    std::cout << "������� Enter ��� ���������� �����������...";
    std::cin.get();

    // �������� ������� � ���������� ������
    request.type = COMPLETE_REQUEST;
    WriteFile(hPipe, &request, sizeof(Request), &bytesWritten, NULL);
}

int main() {
    setlocale(LC_ALL, "Russian");

    std::cout << "������ �������\n";

    // ������������ � �������
    HANDLE hPipe = connectToServer();
    if (hPipe == INVALID_HANDLE_VALUE) {
        std::cout << "������� Enter ��� ����������...";
        std::cin.get();
        return 1;
    }

    std::cout << "����������� � ������� �����������\n";

    bool running = true;
    while (running) {
        int choice;
        std::cout << "\n�������� ��������:\n";
        std::cout << "1 - ����������� ������\n";
        std::cout << "2 - ������ ������\n";
        std::cout << "0 - �����\n";
        std::cout << "��� �����: ";
        std::cin >> choice;

        switch (choice) {
        case 1:
            modifyRecord(hPipe);
            break;
        case 2:
            readRecord(hPipe);
            break;
        case 0:
            running = false;
            break;
        default:
            std::cout << "�������� �����. ���������� �����.\n";
        }
    }

    // ��������� ����������
    CloseHandle(hPipe);
    std::cout << "������ �������� ������\n";

    return 0;
}