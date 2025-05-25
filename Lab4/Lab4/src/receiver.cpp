#include "../include/header.h"
#include "../include/message_utils.h"
#include <vector>
#include <fstream>
#include <sstream>

int main() {
    std::string fileName;
    int messageCount, senderCount;

    // ���� ����� ����� � ���������� ���������
    std::cout << "Enter message file name: ";
    std::cin >> fileName;

    std::cout << "Enter number of messages in file: ";
    std::cin >> messageCount;

    // ������� � �������������� ���� ���������
    if (!message_utils::InitializeMessageFile(fileName, messageCount)) {
        return 1;
    }

    // ������� ������� �������������
    HANDLE fileMutex = CreateMutexW(NULL, FALSE, FILE_MUTEX_NAME.c_str());
    HANDLE emptySlotsSemaphore = CreateSemaphoreW(NULL, messageCount, messageCount, EMPTY_SLOTS_SEMAPHORE_NAME.c_str());
    HANDLE filledSlotsSemaphore = CreateSemaphoreW(NULL, 0, messageCount, FILLED_SLOTS_SEMAPHORE_NAME.c_str());

    if (!fileMutex || !emptySlotsSemaphore || !filledSlotsSemaphore) {
        std::cerr << "Failed to create synchronization objects." << std::endl;
        return 1;
    }

    // ���� ���������� ��������� Sender
    std::cout << "Enter number of Sender processes: ";
    std::cin >> senderCount;

    // ��������� �������� Sender
    auto senderProcesses = message_utils::LaunchSenderProcesses(senderCount, fileName);
    if (senderProcesses.empty()) {
        std::cerr << "Failed to launch any Sender processes." << std::endl;
        return 1;
    }

    // �������� ���� ��������� ������
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
            message_utils::ReadMessagesFromFile(fileName, messageCount);
            break;
        case 'q':
            running = false;
            break;
        default:
            std::cout << "Unknown command." << std::endl;
            break;
        }
    }

    // ��������� ����������� ��������� Sender
    for (HANDLE process : senderProcesses) {
        CloseHandle(process);
    }

    // ��������� ������� �������������
    CloseHandle(fileMutex);
    CloseHandle(emptySlotsSemaphore);
    CloseHandle(filledSlotsSemaphore);

    return 0;
}