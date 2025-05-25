#include <gtest/gtest.h>
#include "../include/header.h"
#include "../include/message_utils.h"
#include <fstream>
#include <cstdio>
#include <string>

class MessageUtilsTest : public ::testing::Test {
protected:
    std::string testFileName;

    void SetUp() override {
        testFileName = "test_messages.bin";
    }

    void TearDown() override {
        // ������� �������� ���� ����� ������� �����
        std::remove(testFileName.c_str());
    }

    // ��������������� ������� ��� �������� �������� � ���������
    void CreateSyncObjects(int messageCount) {
        HANDLE fileMutex = CreateMutexW(NULL, FALSE, FILE_MUTEX_NAME.c_str());
        HANDLE emptySlotsSemaphore = CreateSemaphoreW(NULL, messageCount, messageCount, EMPTY_SLOTS_SEMAPHORE_NAME.c_str());
        HANDLE filledSlotsSemaphore = CreateSemaphoreW(NULL, 0, messageCount, FILLED_SLOTS_SEMAPHORE_NAME.c_str());

        ASSERT_TRUE(fileMutex != NULL);
        ASSERT_TRUE(emptySlotsSemaphore != NULL);
        ASSERT_TRUE(filledSlotsSemaphore != NULL);
    }

    // ��������������� ������� ��� �������� �������� � ���������
    void CloseSyncObjects() {
        HANDLE fileMutex = OpenMutexW(SYNCHRONIZE, FALSE, FILE_MUTEX_NAME.c_str());
        HANDLE emptySlotsSemaphore = OpenSemaphoreW(SEMAPHORE_ALL_ACCESS, FALSE, EMPTY_SLOTS_SEMAPHORE_NAME.c_str());
        HANDLE filledSlotsSemaphore = OpenSemaphoreW(SEMAPHORE_ALL_ACCESS, FALSE, FILLED_SLOTS_SEMAPHORE_NAME.c_str());

        if (fileMutex) CloseHandle(fileMutex);
        if (emptySlotsSemaphore) CloseHandle(emptySlotsSemaphore);
        if (filledSlotsSemaphore) CloseHandle(filledSlotsSemaphore);
    }
};

// ���� ������������� ����� ���������
TEST_F(MessageUtilsTest, InitializeMessageFile) {
    int messageCount = 5;
    bool result = message_utils::InitializeMessageFile(testFileName, messageCount);
    ASSERT_TRUE(result);

    // ���������, ��� ���� ������ � ����� ��������� ������
    std::ifstream file(testFileName, std::ios::binary | std::ios::ate);
    ASSERT_TRUE(file.is_open());

    std::streamsize fileSize = file.tellg();
    EXPECT_EQ(fileSize, messageCount * sizeof(Message));

    // ���������, ��� ��� ��������� ������
    file.seekg(0);
    for (int i = 0; i < messageCount; ++i) {
        Message message;
        file.read(reinterpret_cast<char*>(&message), sizeof(Message));
        EXPECT_TRUE(message.isEmpty);
    }
}

// ���� �� �������� IsSlotEmpty
TEST_F(MessageUtilsTest, IsSlotEmpty) {
    Message emptyMessage;
    EXPECT_TRUE(message_utils::IsSlotEmpty(emptyMessage));

    Message nonEmptyMessage;
    nonEmptyMessage.isEmpty = false;
    strcpy_s(nonEmptyMessage.text, "Test Message");
    EXPECT_FALSE(message_utils::IsSlotEmpty(nonEmptyMessage));
}

// ���� ������ � ������ ���������
TEST_F(MessageUtilsTest, WriteAndReadMessages) {
    int messageCount = 3;

    // �������������� ���� � ������� ������� �������������
    ASSERT_TRUE(message_utils::InitializeMessageFile(testFileName, messageCount));
    CreateSyncObjects(messageCount);

    // ���������� ���������
    std::string msg1 = "Test Message 1";
    std::string msg2 = "Test Message 2";

    // ���� ������ ���������
    EXPECT_TRUE(message_utils::WriteMessageToFile(testFileName, msg1));
    EXPECT_TRUE(message_utils::WriteMessageToFile(testFileName, msg2));

    // ��������� ���������� ���������
    EXPECT_EQ(message_utils::CountFilledMessages(testFileName, messageCount), 2);

    // ��������� ������� �������������
    CloseSyncObjects();
}

// ���� �������� ����������� ���������
TEST_F(MessageUtilsTest, CountFilledMessages) {
    int messageCount = 5;
    ASSERT_TRUE(message_utils::InitializeMessageFile(testFileName, messageCount));

    // ������ ���� 0 ����������� ��������� ����� �������������
    EXPECT_EQ(message_utils::CountFilledMessages(testFileName, messageCount), 0);

    // ������ ���������� ��������� ��� ������������ ��������
    std::fstream file(testFileName, std::ios::binary | std::ios::in | std::ios::out);
    ASSERT_TRUE(file.is_open());

    Message message;
    message.isEmpty = false;
    strcpy_s(message.text, "Test");

    // ���������� 2 ���������
    file.seekp(0);
    file.write(reinterpret_cast<char*>(&message), sizeof(Message));

    file.seekp(2 * sizeof(Message));
    file.write(reinterpret_cast<char*>(&message), sizeof(Message));

    file.close();

    // ��������� �������
    EXPECT_EQ(message_utils::CountFilledMessages(testFileName, messageCount), 2);
}

// ���� �� �������� ������ ������� �������� ���������� ���������
TEST_F(MessageUtilsTest, TooManyMessages) {
    int messageCount = 2;
    ASSERT_TRUE(message_utils::InitializeMessageFile(testFileName, messageCount));
    CreateSyncObjects(messageCount);

    // ���������� ������������ ���������� ���������
    EXPECT_TRUE(message_utils::WriteMessageToFile(testFileName, "Message 1"));
    EXPECT_TRUE(message_utils::WriteMessageToFile(testFileName, "Message 2"));

    // ������ ������ ������ �� ��������, ��� ��� ������ ������ 2
    // �� �� �� ����� ��������� ��� ��������� � �����, ��� ��� WriteMessageToFile ����� ����� ����������
    // ���� �� ����������� ����. ��������� ���� ����.

    // �������
    CloseSyncObjects();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}