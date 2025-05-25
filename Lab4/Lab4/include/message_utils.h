#pragma once

#include "header.h"
#include <vector>
#include <fstream>
#include <string>

// ������� ������ ���������� ��� ������ � �����������
namespace message_utils {
    // ������� ��� �������� � ������������� ��������� ����� ���������
    bool InitializeMessageFile(const std::string& fileName, int messageCount);

    // ������ ��������� Sender
    std::vector<HANDLE> LaunchSenderProcesses(int senderCount, const std::string& fileName);

    // ������ ��������� �� �����
    void ReadMessagesFromFile(const std::string& fileName, int messageCount);

    // ������ ��������� � ����
    bool WriteMessageToFile(const std::string& fileName, const std::string& messageText);

    // ��������, ���� �� ����
    bool IsSlotEmpty(const Message& message);

    // ������� ���������� �������� ��������� � �����
    int CountFilledMessages(const std::string& fileName, int messageCount);
}