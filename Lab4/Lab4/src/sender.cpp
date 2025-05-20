#include "../include/header.h"
#include <fstream>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: sender.exe <message_file_name> <sender_id>" << std::endl;
        return 1;
    }

    std::string fileName = argv[1];
    int senderId = std::stoi(argv[2]);

    HANDLE fileMutex = OpenMutexW(SYNCHRONIZE, FALSE, FILE_MUTEX_NAME.c_str());
    HANDLE emptySlotsSemaphore = OpenSemaphoreW(SEMAPHORE_ALL_ACCESS, FALSE, EMPTY_SLOTS_SEMAPHORE_NAME.c_str());
    HANDLE filledSlotsSemaphore = OpenSemaphoreW(SEMAPHORE_ALL_ACCESS, FALSE, FILLED_SLOTS_SEMAPHORE_NAME.c_str());

    if (!fileMutex || !emptySlotsSemaphore || !filledSlotsSemaphore) {
        std::cerr << "Failed to open synchronization objects." << std::endl;
        return 1;
    }

    // Сигнализируем о готовности к работе
    std::wstring eventName = SENDER_READY_EVENT_PREFIX + std::to_wstring(senderId);
    HANDLE readyEvent = OpenEventW(EVENT_MODIFY_STATE, FALSE, eventName.c_str());
    if (readyEvent) {
        SetEvent(readyEvent);
        CloseHandle(readyEvent);
    }

    std::cout << "Sender " << senderId << " is ready." << std::endl;

    bool running = true;
    while (running) {
        std::cout << "\nCommands:\n"
            << "s - Send message\n"
            << "q - Quit\n"
            << "Enter command: ";

        char command;
        std::cin >> command;
        std::cin.ignore();

        switch (command) {
        case 's': {
            std::cout << "Enter message (max " << MAX_MESSAGE_LENGTH << " chars): ";
            std::string messageText;
            std::getline(std::cin, messageText);

            if (messageText.length() > MAX_MESSAGE_LENGTH) {
                messageText.resize(MAX_MESSAGE_LENGTH);
                std::cout << "Message truncated to " << MAX_MESSAGE_LENGTH << " characters." << std::endl;
            }

            // Ждем доступный слот
            if (WaitForSingleObject(emptySlotsSemaphore, INFINITE) != WAIT_OBJECT_0) {
                std::cerr << "Failed to wait for empty slot." << std::endl;
                continue;
            }

            // Захватываем мьютекс
            WaitForSingleObject(fileMutex, INFINITE);

            std::fstream file(fileName, std::ios::binary | std::ios::in | std::ios::out);
            if (!file) {
                std::cerr << "Failed to open message file." << std::endl;
                ReleaseMutex(fileMutex);
                ReleaseSemaphore(emptySlotsSemaphore, 1, NULL);
                continue;
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
                std::cout << "Message sent successfully." << std::endl;
            }
            else {
                ReleaseSemaphore(emptySlotsSemaphore, 1, NULL);
                std::cout << "No empty slots found." << std::endl;
            }
            break;
        }
        case 'q':
            running = false;
            break;
        default:
            std::cout << "Unknown command." << std::endl;
            break;
        }
    }

    CloseHandle(fileMutex);
    CloseHandle(emptySlotsSemaphore);
    CloseHandle(filledSlotsSemaphore);

    return 0;
}