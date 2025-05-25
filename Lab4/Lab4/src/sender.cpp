#include "../include/header.h"
#include "../include/message_utils.h"
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

            if (message_utils::WriteMessageToFile(fileName, messageText)) {
                std::cout << "Message sent successfully." << std::endl;
            }
            else {
                std::cout << "Failed to send message." << std::endl;
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