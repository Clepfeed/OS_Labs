#include <iostream>
#include <windows.h>
#include <string>
#include "../include/common.h"

HANDLE connectToServer() {
    HANDLE hPipe;

    // Пытаемся подключиться к именованному каналу
    while (true) {
        hPipe = CreateFile(
            PIPE_NAME.c_str(),      // имя канала
            GENERIC_READ |          // режим доступа
            GENERIC_WRITE,
            0,                      // режим совместного использования
            NULL,                   // атрибуты безопасности
            OPEN_EXISTING,          // действие при создании
            0,                      // атрибуты и флаги
            NULL);                  // шаблон файла

        if (hPipe != INVALID_HANDLE_VALUE) {
            break;
        }

        if (GetLastError() != ERROR_PIPE_BUSY) {
            std::cerr << "Ошибка подключения к серверу: " << GetLastError() << std::endl;
            return INVALID_HANDLE_VALUE;
        }

        if (!WaitNamedPipe(PIPE_NAME.c_str(), 5000)) {
            std::cerr << "Таймаут ожидания канала\n";
            return INVALID_HANDLE_VALUE;
        }
    }

    DWORD dwMode = PIPE_READMODE_MESSAGE;
    if (!SetNamedPipeHandleState(hPipe, &dwMode, NULL, NULL)) {
        std::cerr << "Ошибка установки режима канала\n";
        CloseHandle(hPipe);
        return INVALID_HANDLE_VALUE;
    }

    return hPipe;
}

void displayEmployee(const Employee& emp) {
    std::cout << "ID: " << emp.num << "\n";
    std::cout << "Имя: " << emp.name << "\n";
    std::cout << "Часы: " << emp.hours << "\n";
}

// Функция для чтения записи
void readRecord(HANDLE hPipe) {
    DWORD bytesWritten, bytesRead;
    Request request;
    Response response;

    request.type = READ_REQUEST;

    std::cout << "Введите ID сотрудника: ";
    std::cin >> request.employeeId;

    // Отправляем запрос на чтение
    if (!WriteFile(hPipe, &request, sizeof(Request), &bytesWritten, NULL)) {
        std::cerr << "Ошибка отправки запроса\n";
        return;
    }

    // Получаем ответ
    if (!ReadFile(hPipe, &response, sizeof(Response), &bytesRead, NULL)) {
        std::cerr << "Ошибка получения ответа\n";
        return;
    }

    if (response.success) {
        std::cout << "\nДанные сотрудника:\n";
        displayEmployee(response.data);
    }
    else {
        std::cout << "Сотрудник с ID " << request.employeeId << " не найден\n";
    }

    std::cout << "Нажмите Enter для завершения чтения...";
    std::cin.ignore();
    std::cin.get();

    // Сообщаем серверу о завершении чтения
    request.type = COMPLETE_REQUEST;
    WriteFile(hPipe, &request, sizeof(Request), &bytesWritten, NULL);
}

// Функция для модификации записи
void modifyRecord(HANDLE hPipe) {
    DWORD bytesWritten, bytesRead;
    Request request;
    Response response;

    request.type = WRITE_REQUEST;

    std::cout << "Введите ID сотрудника для модификации: ";
    std::cin >> request.employeeId;

    // Отправляем запрос на запись
    if (!WriteFile(hPipe, &request, sizeof(Request), &bytesWritten, NULL)) {
        std::cerr << "Ошибка отправки запроса\n";
        return;
    }

    // Получаем ответ
    if (!ReadFile(hPipe, &response, sizeof(Response), &bytesRead, NULL)) {
        std::cerr << "Ошибка получения ответа\n";
        return;
    }

    if (!response.success) {
        std::cout << "Сотрудник с ID " << request.employeeId << " не найден или запись заблокирована\n";
        return;
    }

    std::cout << "\nТекущие данные сотрудника:\n";
    displayEmployee(response.data);

    // Запрашиваем новые значения
    request.data = response.data;
    std::cout << "\nВведите новые значения (Enter - оставить без изменений):\n";

    std::cout << "Имя [" << request.data.name << "]: ";
    std::cin.ignore();
    std::string newName;
    std::getline(std::cin, newName);
    if (!newName.empty()) {
        strncpy_s(request.data.name, newName.c_str(), sizeof(request.data.name) - 1);
    }

    std::cout << "Часы [" << request.data.hours << "]: ";
    std::string hoursStr;
    std::getline(std::cin, hoursStr);
    if (!hoursStr.empty()) {
        request.data.hours = std::stod(hoursStr);
    }

    // Отправляем измененные данные
    if (!WriteFile(hPipe, &request, sizeof(Request), &bytesWritten, NULL)) {
        std::cerr << "Ошибка отправки модифицированных данных\n";
        return;
    }

    std::cout << "\nДанные успешно обновлены\n";
    std::cout << "Нажмите Enter для завершения модификации...";
    std::cin.get();

    // Сообщаем серверу о завершении записи
    request.type = COMPLETE_REQUEST;
    WriteFile(hPipe, &request, sizeof(Request), &bytesWritten, NULL);
}

int main() {
    setlocale(LC_ALL, "Russian");

    std::cout << "Клиент запущен\n";

    // Подключаемся к серверу
    HANDLE hPipe = connectToServer();
    if (hPipe == INVALID_HANDLE_VALUE) {
        std::cout << "Нажмите Enter для завершения...";
        std::cin.get();
        return 1;
    }

    std::cout << "Подключение к серверу установлено\n";

    bool running = true;
    while (running) {
        int choice;
        std::cout << "\nВыберите операцию:\n";
        std::cout << "1 - Модификация записи\n";
        std::cout << "2 - Чтение записи\n";
        std::cout << "0 - Выход\n";
        std::cout << "Ваш выбор: ";
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
            std::cout << "Неверный выбор. Попробуйте снова.\n";
        }
    }

    // Закрываем соединение
    CloseHandle(hPipe);
    std::cout << "Клиент завершил работу\n";

    return 0;
}