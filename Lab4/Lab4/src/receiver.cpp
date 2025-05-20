#include "../include/header.h"
#include <vector>
#include <fstream>
#include <sstream>

// Функция для создания и инициализации бинарного файла сообщений
bool InitializeMessageFile(const std::string& fileName, int messageCount) {
    std::ofstream file(fileName, std::ios::binary | std::ios::trunc);
    if (!file) {
        std::cerr << "Failed to create message file." << std::endl;
        return false;
    }

    // Инициализация файла пустыми сообщениями
    Message emptyMessage;
    for (int i = 0; i < messageCount; ++i) {
        file.write(reinterpret_cast<char*>(&emptyMessage), sizeof(Message));
    }

    file.close();
    return true;
}

// Запуск процессов Sender
std::vector<HANDLE> LaunchSenderProcesses(int senderCount, const std::string& fileName) {
    std::vector<HANDLE> senderProcesses;
    std::vector<HANDLE> readyEvents;

    for (int i = 0; i < senderCount; ++i) {
        std::wstring eventName = SENDER_READY_EVENT_PREFIX + std::to_wstring(i);
        HANDLE readyEvent = CreateEventW(NULL, TRUE, FALSE, eventName.c_str());
        if (readyEvent == NULL) {
            std::cerr << "Failed to create ready event for Sender " << i << std::endl;
            continue;
        }
        readyEvents.push_back(readyEvent);

        std::wstringstream cmdLine;
        cmdLine << L"sender.exe " << std::wstring(fileName.begin(), fileName.end()) << L" " << i;

        STARTUPINFOW si = { sizeof(STARTUPINFOW) };
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        ZeroMemory(&pi, sizeof(pi));
        si.cb = sizeof(si);

        if (CreateProcessW(
            NULL,
            const_cast<LPWSTR>(cmdLine.str().c_str()),
            NULL,
            NULL,
            FALSE,
            CREATE_NEW_CONSOLE,
            NULL,
            NULL,
            &si,
            &pi
        )) {
            senderProcesses.push_back(pi.hProcess);
            CloseHandle(pi.hThread);
        }
        else {
            std::cerr << "Failed to create Sender process " << i << ": " << GetLastError() << std::endl;
            CloseHandle(readyEvent);
            readyEvents.pop_back();
        }
    }

    if (!readyEvents.empty()) {
        std::cout << "Waiting for all Senders to initialize..." << std::endl;
        WaitForMultipleObjects(static_cast<DWORD>(readyEvents.size()), readyEvents.data(), TRUE, INFINITE);
        std::cout << "All Senders are ready!" << std::endl;
    }

    for (HANDLE event : readyEvents) {
        CloseHandle(event);
    }

    return senderProcesses;
}


// Чтение сообщений из файла
void ReadMessagesFromFile(const std::string& fileName, int messageCount) {
    HANDLE fileMutex = OpenMutexW(SYNCHRONIZE, FALSE, FILE_MUTEX_NAME.c_str());
    HANDLE filledSlotsSemaphore = OpenSemaphoreW(SEMAPHORE_ALL_ACCESS, FALSE, FILLED_SLOTS_SEMAPHORE_NAME.c_str());

    if (!fileMutex || !filledSlotsSemaphore) {
        std::cerr << "Failed to open synchronization objects for reading." << std::endl;
        return;
    }

    std::fstream file(fileName, std::ios::binary | std::ios::in | std::ios::out);
    if (!file) {
        std::cerr << "Failed to open message file for reading." << std::endl;
        return;
    }

    WaitForSingleObject(fileMutex, INFINITE);

    bool messagesFound = false;
    Message message;

    // Читаем все сообщения из файла
    for (int i = 0; i < messageCount; ++i) {
        file.seekg(i * sizeof(Message));
        file.read(reinterpret_cast<char*>(&message), sizeof(Message));

        if (!message.isEmpty) {
            std::cout << "Message " << i + 1 << ": " << message.text << std::endl;
            messagesFound = true;
        }
    }

    ReleaseMutex(fileMutex);

    if (!messagesFound) {
        std::cout << "No messages in file." << std::endl;
    }

    file.close();
    CloseHandle(fileMutex);
    CloseHandle(filledSlotsSemaphore);
}

int main() {
    std::string fileName;
    int messageCount, senderCount;

    // Ввод имени файла и количества сообщений
    std::cout << "Enter message file name: ";
    std::cin >> fileName;

    std::cout << "Enter number of messages in file: ";
    std::cin >> messageCount;

    // Создаем и инициализируем файл сообщений
    if (!InitializeMessageFile(fileName, messageCount)) {
        return 1;
    }

    // Создаем объекты синхронизации
    HANDLE fileMutex = CreateMutexW(NULL, FALSE, FILE_MUTEX_NAME.c_str());
    HANDLE emptySlotsSemaphore = CreateSemaphoreW(NULL, messageCount, messageCount, EMPTY_SLOTS_SEMAPHORE_NAME.c_str());
    HANDLE filledSlotsSemaphore = CreateSemaphoreW(NULL, 0, messageCount, FILLED_SLOTS_SEMAPHORE_NAME.c_str());

    if (!fileMutex || !emptySlotsSemaphore || !filledSlotsSemaphore) {
        std::cerr << "Failed to create synchronization objects." << std::endl;
        return 1;
    }

    // Ввод количества процессов Sender
    std::cout << "Enter number of Sender processes: ";
    std::cin >> senderCount;

    // Запускаем процессы Sender
    auto senderProcesses = LaunchSenderProcesses(senderCount, fileName);
    if (senderProcesses.empty()) {
        std::cerr << "Failed to launch any Sender processes." << std::endl;
        return 1;
    }

    // Основной цикл обработки команд
    bool running = true;
    while (running) {
        std::cout << "\nCommands:\n"
            << "r - Read messages\n"
            << "q - Quit\n"
            << "Enter command: ";

        char command;
        std::cin >> command;

        switch (command) {
        case 'r':
            ReadMessagesFromFile(fileName, messageCount);
            break;
        case 'q':
            running = false;
            break;
        default:
            std::cout << "Unknown command." << std::endl;
            break;
        }
    }

    // Закрываем дескрипторы процессов Sender
    for (HANDLE process : senderProcesses) {
        CloseHandle(process);
    }

    // Закрываем объекты синхронизации
    CloseHandle(fileMutex);
    CloseHandle(emptySlotsSemaphore);
    CloseHandle(filledSlotsSemaphore);

    return 0;
}