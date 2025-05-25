#include "message_utils.h"
#include <sstream>

namespace message_utils {

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

    bool WriteMessageToFile(const std::string& fileName, const std::string& messageText) {
        HANDLE fileMutex = OpenMutexW(SYNCHRONIZE, FALSE, FILE_MUTEX_NAME.c_str());
        HANDLE emptySlotsSemaphore = OpenSemaphoreW(SEMAPHORE_ALL_ACCESS, FALSE, EMPTY_SLOTS_SEMAPHORE_NAME.c_str());
        HANDLE filledSlotsSemaphore = OpenSemaphoreW(SEMAPHORE_ALL_ACCESS, FALSE, FILLED_SLOTS_SEMAPHORE_NAME.c_str());

        if (!fileMutex || !emptySlotsSemaphore || !filledSlotsSemaphore) {
            std::cerr << "Failed to open synchronization objects." << std::endl;
            return false;
        }

        // Ждем доступный слот
        if (WaitForSingleObject(emptySlotsSemaphore, INFINITE) != WAIT_OBJECT_0) {
            std::cerr << "Failed to wait for empty slot." << std::endl;
            return false;
        }

        // Захватываем мьютекс
        WaitForSingleObject(fileMutex, INFINITE);

        std::fstream file(fileName, std::ios::binary | std::ios::in | std::ios::out);
        if (!file) {
            std::cerr << "Failed to open message file." << std::endl;
            ReleaseMutex(fileMutex);
            ReleaseSemaphore(emptySlotsSemaphore, 1, NULL);
            return false;
        }

        // Определяем размер файла и количество сообщений
        file.seekg(0, std::ios::end);
        int fileSize = file.tellg();
        int messageCount = fileSize / sizeof(Message);

        // Ищем пустой слот
        Message message;
        bool slotFound = false;

        for (int i = 0; i < messageCount; ++i) {
            file.seekg(i * sizeof(Message));
            file.read(reinterpret_cast<char*>(&message), sizeof(Message));

            if (message.isEmpty) {
                // Нашли пустой слот, записываем сообщение
                memset(&message, 0, sizeof(Message)); // Очищаем структуру
                strncpy_s(message.text, messageText.c_str(), MAX_MESSAGE_LENGTH);
                message.isEmpty = false;

                file.seekp(i * sizeof(Message));
                file.write(reinterpret_cast<char*>(&message), sizeof(Message));

                slotFound = true;
                break;
            }
        }

        file.close();
        ReleaseMutex(fileMutex);

        if (slotFound) {
            ReleaseSemaphore(filledSlotsSemaphore, 1, NULL);
            return true;
        }
        else {
            ReleaseSemaphore(emptySlotsSemaphore, 1, NULL);
            return false;
        }
    }

    bool IsSlotEmpty(const Message& message) {
        return message.isEmpty;
    }

    int CountFilledMessages(const std::string& fileName, int messageCount) {
        std::ifstream file(fileName, std::ios::binary);
        if (!file) {
            return -1; // Ошибка открытия файла
        }

        int filledCount = 0;
        Message message;

        for (int i = 0; i < messageCount; ++i) {
            file.seekg(i * sizeof(Message));
            file.read(reinterpret_cast<char*>(&message), sizeof(Message));

            if (!message.isEmpty) {
                filledCount++;
            }
        }

        file.close();
        return filledCount;
    }
}