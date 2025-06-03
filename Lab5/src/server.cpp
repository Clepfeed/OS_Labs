#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <windows.h>
#include <process.h>
#include "../include/common.h"

// Глобальные переменные
std::string filename;
std::vector<Employee> employees;
std::vector<bool> recordLocked;
std::vector<int> readCount;
std::vector<HANDLE> recordMutexes;
HANDLE globalMutex;

// Функция создания бинарного файла
void createBinaryFile() {
    std::cout << "Введите имя файла: ";
    std::cin >> filename;

    int count;
    std::cout << "Введите количество сотрудников: ";
    std::cin >> count;

    employees.resize(count);
    recordLocked.resize(count, false);
    readCount.resize(count, 0);

    // Создаем мьютексы для каждой записи
    recordMutexes.resize(count);
    for (int i = 0; i < count; i++) {
        recordMutexes[i] = CreateMutex(NULL, FALSE, NULL);
    }

    // Создаем глобальный мьютекс
    globalMutex = CreateMutex(NULL, FALSE, NULL);

    std::cout << "Введите данные сотрудников:\n";
    for (int i = 0; i < count; i++) {
        std::cout << "Сотрудник #" << i + 1 << ":\n";
        std::cout << "ID: ";
        std::cin >> employees[i].num;
        std::cout << "Имя (до 9 символов): ";
        std::cin >> employees[i].name;
        std::cout << "Часы: ";
        std::cin >> employees[i].hours;
    }

    // Сохранение в файл
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Ошибка создания файла\n";
        return;
    }

    for (const auto& emp : employees) {
        file.write(reinterpret_cast<const char*>(&emp), sizeof(Employee));
    }

    file.close();
    std::cout << "Файл успешно создан\n";
}

// Функция вывода файла на консоль
void displayFile() {
    std::cout << "\nСодержимое файла " << filename << ":\n";
    std::cout << "ID\tИмя\t\tЧасы\n";
    std::cout << "------------------------\n";

    for (const auto& emp : employees) {
        std::cout << emp.num << "\t" << emp.name << "\t\t" << emp.hours << "\n";
    }
    std::cout << "------------------------\n";
}

// Функция поиска сотрудника по ID
int findEmployeeIndex(int id) {
    for (size_t i = 0; i < employees.size(); i++) {
        if (employees[i].num == id) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

// Функция обработки клиентского запроса
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
                    // Запись заблокирована или читается
                    response.success = false;
                    ReleaseMutex(globalMutex);
                    ReleaseMutex(recordMutexes[index]);
                    WriteFile(hPipe, &response, sizeof(Response), &bytesWritten, NULL);
                }
                else {
                    // Блокируем запись
                    recordLocked[index] = true;
                    response.success = true;
                    response.data = employees[index];
                    ReleaseMutex(globalMutex);

                    WriteFile(hPipe, &response, sizeof(Response), &bytesWritten, NULL);

                    ReadFile(hPipe, &request, sizeof(Request), &bytesRead, NULL);

                    WaitForSingleObject(globalMutex, INFINITE);
                    employees[index] = request.data;
                    recordLocked[index] = false;

                    // Обновляем файл
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

// Функция для создания и запуска клиентского процесса
void startClientProcess() {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Создаем процесс клиента
    if (!CreateProcess(
        ".\\Client.exe",   // Имя исполняемого файла
        NULL,          // Командная строка
        NULL,          // Атрибуты защиты процесса
        NULL,          // Атрибуты защиты потока
        FALSE,         // Наследование дескрипторов
        CREATE_NEW_CONSOLE, // Флаги создания: новая консоль
        NULL,          // Блок среды родительского процесса
        NULL,          // Текущий каталог
        &si,           // STARTUPINFO
        &pi)           // PROCESS_INFORMATION
        ) {
        std::cerr << "Ошибка создания клиентского процесса: " << GetLastError() << std::endl;
        return;
    }

    // Закрываем дескрипторы процесса и потока
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

// Основной цикл сервера
void serverLoop() {
    int clientCount;
    std::cout << "Введите количество клиентов: ";
    std::cin >> clientCount;

    for (int i = 0; i < clientCount; i++) {
        startClientProcess();
    }

    HANDLE hPipe;
    HANDLE* hThreads = new HANDLE[clientCount];
    int threadCount = 0;

    while (threadCount < clientCount) {
        hPipe = CreateNamedPipe(
            PIPE_NAME.c_str(),              // имя канала
            PIPE_ACCESS_DUPLEX,             // двунаправленный канал
            PIPE_TYPE_MESSAGE |             // передача сообщений
            PIPE_READMODE_MESSAGE |         // чтение сообщений
            PIPE_WAIT,                      // блокирующий режим
            PIPE_UNLIMITED_INSTANCES,       // максимальное количество экземпляров
            sizeof(Response),               // размер выходного буфера
            sizeof(Request),                // размер входного буфера
            0,                              // таймаут (0 = по умолчанию)
            NULL);                          // атрибуты безопасности

        if (hPipe == INVALID_HANDLE_VALUE) {
            std::cerr << "Ошибка создания именованного канала: " << GetLastError() << std::endl;
            break;
        }

        // Ожидаем подключения клиента
        if (ConnectNamedPipe(hPipe, NULL) || GetLastError() == ERROR_PIPE_CONNECTED) {
            // Создаем поток для обработки запросов клиента
            hThreads[threadCount] = CreateThread(
                NULL,                   // атрибуты безопасности
                0,                      // размер стека
                (LPTHREAD_START_ROUTINE)handleClientRequest,
                (LPVOID)hPipe,          // аргумент функции потока
                0,                      // флаги создания
                NULL);                  // идентификатор потока

            if (hThreads[threadCount] == NULL) {
                std::cerr << "Ошибка создания потока: " << GetLastError() << std::endl;
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

    std::cout << "Нажмите Enter для завершения...";
    std::cin.ignore();
    std::cin.get();

    for (HANDLE mutex : recordMutexes) {
        CloseHandle(mutex);
    }
    CloseHandle(globalMutex);

    return 0;
}