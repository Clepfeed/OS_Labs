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
        // Чтение запроса от клиента
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
                // Захватываем мьютекс записи
                WaitForSingleObject(recordMutexes[index], INFINITE);

                // Захватываем глобальный мьютекс для проверки состояния
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

                    // Ждем модифицированные данные
                    ReadFile(hPipe, &request, sizeof(Request), &bytesRead, NULL);

                    // Снова захватываем глобальный мьютекс для обновления данных
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
            // Сотрудник не найден
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

    // Создаем и запускаем клиентские процессы
    for (int i = 0; i < clientCount; i++) {
        startClientProcess();
    }

    // Создаем именованный канал
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

    // Ожидаем завершения всех потоков
    WaitForMultipleObjects(threadCount, hThreads, TRUE, INFINITE);

    // Освобождаем ресурсы
    for (int i = 0; i < threadCount; i++) {
        CloseHandle(hThreads[i]);
    }
    delete[] hThreads;
}

#ifdef TEST_BUILD

// Helper function to set up global server state for testing
// This replicates parts of createBinaryFile's initialization logic without file I/O or console I/O.
void server_test_setup(const std::vector<Employee>& initial_employees_data) {
    filename = "test_employee_file.bin"; // Use a dedicated test file name
    employees = initial_employees_data;
    size_t count = employees.size();

    recordLocked.assign(count, false);
    readCount.assign(count, 0);

    // Clean up any previous mutexes if tests are run sequentially in one instance
    for (HANDLE& mutex_handle : recordMutexes) {
        if (mutex_handle != NULL) {
            CloseHandle(mutex_handle);
            mutex_handle = NULL;
        }
    }
    recordMutexes.clear(); // Clear the vector of handles

    if (globalMutex != NULL) {
        CloseHandle(globalMutex);
        globalMutex = NULL;
    }

    recordMutexes.resize(count);
    for (size_t i = 0; i < count; ++i) {
        recordMutexes[i] = CreateMutex(NULL, FALSE, NULL);
        // In a real test, you might want to ASSERT_NE(nullptr, recordMutexes[i]) here
        // but this code is part of the source, not the test itself.
    }
    globalMutex = CreateMutex(NULL, FALSE, NULL);
    // ASSERT_NE(nullptr, globalMutex);
}

// Helper function to clean up global server state after testing
void server_test_teardown() {
    for (HANDLE& mutex_handle : recordMutexes) {
        if (mutex_handle != NULL) {
            CloseHandle(mutex_handle);
            mutex_handle = NULL;
        }
    }
    recordMutexes.clear();

    if (globalMutex != NULL) {
        CloseHandle(globalMutex);
        globalMutex = NULL;
    }

    employees.clear();
    recordLocked.clear();
    readCount.clear();

    // remove(filename.c_str()); // Optionally delete the test file
}

#endif // TEST_BUILD

#ifndef TEST_BUILD

int main() {
    setlocale(LC_ALL, "Russian");

    // Создаем бинарный файл
    createBinaryFile();

    // Выводим содержимое файла
    displayFile();

    // Запускаем основной цикл сервера
    serverLoop();

    // После завершения клиентов показываем обновленный файл
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
#endif