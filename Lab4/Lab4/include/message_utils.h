#pragma once

#include "header.h"
#include <vector>
#include <fstream>
#include <string>

// Функции общего назначения для работы с сообщениями
namespace message_utils {
    // Функция для создания и инициализации бинарного файла сообщений
    bool InitializeMessageFile(const std::string& fileName, int messageCount);

    // Запуск процессов Sender
    std::vector<HANDLE> LaunchSenderProcesses(int senderCount, const std::string& fileName);

    // Чтение сообщений из файла
    void ReadMessagesFromFile(const std::string& fileName, int messageCount);

    // Запись сообщения в файл
    bool WriteMessageToFile(const std::string& fileName, const std::string& messageText);

    // Проверка, пуст ли слот
    bool IsSlotEmpty(const Message& message);

    // Подсчет количества непустых сообщений в файле
    int CountFilledMessages(const std::string& fileName, int messageCount);
}