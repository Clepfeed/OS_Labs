#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <windows.h>
#include <process.h>
#include "../include/common.h"

// ���������� ����������
std::string filename;
std::vector<Employee> employees;
std::vector<bool> recordLocked;
std::vector<int> readCount;
std::vector<HANDLE> recordMutexes;
HANDLE globalMutex;

// ������� �������� ��������� �����
void createBinaryFile() {
    std::cout << "������� ��� �����: ";
    std::cin >> filename;

    int count;
    std::cout << "������� ���������� �����������: ";
    std::cin >> count;

    employees.resize(count);
    recordLocked.resize(count, false);
    readCount.resize(count, 0);

    // ������� �������� ��� ������ ������
    recordMutexes.resize(count);
    for (int i = 0; i < count; i++) {
        recordMutexes[i] = CreateMutex(NULL, FALSE, NULL);
    }

    // ������� ���������� �������
    globalMutex = CreateMutex(NULL, FALSE, NULL);

    std::cout << "������� ������ �����������:\n";
    for (int i = 0; i < count; i++) {
        std::cout << "��������� #" << i + 1 << ":\n";
        std::cout << "ID: ";
        std::cin >> employees[i].num;
        std::cout << "��� (�� 9 ��������): ";
        std::cin >> employees[i].name;
        std::cout << "����: ";
        std::cin >> employees[i].hours;
    }

    // ���������� � ����
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "������ �������� �����\n";
        return;
    }

    for (const auto& emp : employees) {
        file.write(reinterpret_cast<const char*>(&emp), sizeof(Employee));
    }

    file.close();
    std::cout << "���� ������� ������\n";
}

// ������� ������ ����� �� �������
void displayFile() {
    std::cout << "\n���������� ����� " << filename << ":\n";
    std::cout << "ID\t���\t\t����\n";
    std::cout << "------------------------\n";

    for (const auto& emp : employees) {
        std::cout << emp.num << "\t" << emp.name << "\t\t" << emp.hours << "\n";
    }
    std::cout << "------------------------\n";
}

// ������� ������ ���������� �� ID
int findEmployeeIndex(int id) {
    for (size_t i = 0; i < employees.size(); i++) {
        if (employees[i].num == id) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

// ������� ��������� ����������� �������
void handleClientRequest(HANDLE hPipe) {
    DWORD bytesRead, bytesWritten;
    Request request;
    Response response;
    bool connected = true;

    while (connected) {
        if (!ReadFile(hPipe, &request, sizeof(Request), &bytesRead, NULL) || bytesRead == 0) {
            break;
        }

        int index = findEmployeeIndex(request.employeeId);
        response.success = (index != -1);

        if (response.success) {
            switch (request.type) {
            case READ_REQUEST:
                WaitForSingleObject(globalMutex, INFINITE);
                if (recordLocked[index]) {
                    response.success = false;
                    ReleaseMutex(globalMutex);
                }
                else {
                    readCount[index]++;
                    response.success = true;
                    response.data = employees[index];
                    ReleaseMutex(globalMutex);
                }
                WriteFile(hPipe, &response, sizeof(Response), &bytesWritten, NULL);
                break;

            case WRITE_REQUEST:
                WaitForSingleObject(recordMutexes[index], INFINITE);

                WaitForSingleObject(globalMutex, INFINITE);

                if (recordLocked[index] || readCount[index] > 0) {
                    // ������ ������������� ��� ��������
                    response.success = false;
                    ReleaseMutex(globalMutex);
                    ReleaseMutex(recordMutexes[index]);
                    WriteFile(hPipe, &response, sizeof(Response), &bytesWritten, NULL);
                }
                else {
                    // ��������� ������
                    recordLocked[index] = true;
                    response.success = true;
                    response.data = employees[index];
                    ReleaseMutex(globalMutex);

                    WriteFile(hPipe, &response, sizeof(Response), &bytesWritten, NULL);

                    ReadFile(hPipe, &request, sizeof(Request), &bytesRead, NULL);

                    WaitForSingleObject(globalMutex, INFINITE);
                    employees[index] = request.data;
                    recordLocked[index] = false;

                    // ��������� ����
                    std::ofstream file(filename, std::ios::binary | std::ios::out);
                    for (const auto& emp : employees) {
                        file.write(reinterpret_cast<const char*>(&emp), sizeof(Employee));
                    }
                    file.close();

                    ReleaseMutex(globalMutex);
                    ReleaseMutex(recordMutexes[index]);
                }
                break;

            case COMPLETE_REQUEST:
                WaitForSingleObject(globalMutex, INFINITE);
                if (readCount[index] > 0) {
                    readCount[index]--;
                }
                if (recordLocked[index]) {
                    recordLocked[index] = false;
                }
                ReleaseMutex(globalMutex);
                break;

            default:
                break;
            }
        }
        else {
            WriteFile(hPipe, &response, sizeof(Response), &bytesWritten, NULL);
        }
    }

    FlushFileBuffers(hPipe);
    DisconnectNamedPipe(hPipe);
    CloseHandle(hPipe);
}

// ������� ��� �������� � ������� ����������� ��������
void startClientProcess() {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // ������� ������� �������
    if (!CreateProcess(
        ".\\Client.exe",   // ��� ������������ �����
        NULL,          // ��������� ������
        NULL,          // �������� ������ ��������
        NULL,          // �������� ������ ������
        FALSE,         // ������������ ������������
        CREATE_NEW_CONSOLE, // ����� ��������: ����� �������
        NULL,          // ���� ����� ������������� ��������
        NULL,          // ������� �������
        &si,           // STARTUPINFO
        &pi)           // PROCESS_INFORMATION
        ) {
        std::cerr << "������ �������� ����������� ��������: " << GetLastError() << std::endl;
        return;
    }

    // ��������� ����������� �������� � ������
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

// �������� ���� �������
void serverLoop() {
    int clientCount;
    std::cout << "������� ���������� ��������: ";
    std::cin >> clientCount;

    for (int i = 0; i < clientCount; i++) {
        startClientProcess();
    }

    HANDLE hPipe;
    HANDLE* hThreads = new HANDLE[clientCount];
    int threadCount = 0;

    while (threadCount < clientCount) {
        hPipe = CreateNamedPipe(
            PIPE_NAME.c_str(),              // ��� ������
            PIPE_ACCESS_DUPLEX,             // ��������������� �����
            PIPE_TYPE_MESSAGE |             // �������� ���������
            PIPE_READMODE_MESSAGE |         // ������ ���������
            PIPE_WAIT,                      // ����������� �����
            PIPE_UNLIMITED_INSTANCES,       // ������������ ���������� �����������
            sizeof(Response),               // ������ ��������� ������
            sizeof(Request),                // ������ �������� ������
            0,                              // ������� (0 = �� ���������)
            NULL);                          // �������� ������������

        if (hPipe == INVALID_HANDLE_VALUE) {
            std::cerr << "������ �������� ������������ ������: " << GetLastError() << std::endl;
            break;
        }

        // ������� ����������� �������
        if (ConnectNamedPipe(hPipe, NULL) || GetLastError() == ERROR_PIPE_CONNECTED) {
            // ������� ����� ��� ��������� �������� �������
            hThreads[threadCount] = CreateThread(
                NULL,                   // �������� ������������
                0,                      // ������ �����
                (LPTHREAD_START_ROUTINE)handleClientRequest,
                (LPVOID)hPipe,          // �������� ������� ������
                0,                      // ����� ��������
                NULL);                  // ������������� ������

            if (hThreads[threadCount] == NULL) {
                std::cerr << "������ �������� ������: " << GetLastError() << std::endl;
                CloseHandle(hPipe);
            }
            else {
                threadCount++;
            }
        }
        else {
            CloseHandle(hPipe);
        }
    }

    WaitForMultipleObjects(threadCount, hThreads, TRUE, INFINITE);

    for (int i = 0; i < threadCount; i++) {
        CloseHandle(hThreads[i]);
    }
    delete[] hThreads;
}


int main() {
    setlocale(LC_ALL, "Russian");

    createBinaryFile();

    displayFile();

    serverLoop();

    displayFile();

    std::cout << "������� Enter ��� ����������...";
    std::cin.ignore();
    std::cin.get();

    for (HANDLE mutex : recordMutexes) {
        CloseHandle(mutex);
    }
    CloseHandle(globalMutex);

    return 0;
}