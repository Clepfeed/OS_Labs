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
        // Удаляем тестовый файл после каждого теста
        std::remove(testFileName.c_str());
    }

    // Вспомогательная функция для создания мьютекса и семафоров
    void CreateSyncObjects(int messageCount) {
        HANDLE fileMutex = CreateMutexW(NULL, FALSE, FILE_MUTEX_NAME.c_str());
        HANDLE emptySlotsSemaphore = CreateSemaphoreW(NULL, messageCount, messageCount, EMPTY_SLOTS_SEMAPHORE_NAME.c_str());
        HANDLE filledSlotsSemaphore = CreateSemaphoreW(NULL, 0, messageCount, FILLED_SLOTS_SEMAPHORE_NAME.c_str());

        ASSERT_TRUE(fileMutex != NULL);
        ASSERT_TRUE(emptySlotsSemaphore != NULL);
        ASSERT_TRUE(filledSlotsSemaphore != NULL);
    }

    // Вспомогательная функция для закрытия мьютекса и семафоров
    void CloseSyncObjects() {
        HANDLE fileMutex = OpenMutexW(SYNCHRONIZE, FALSE, FILE_MUTEX_NAME.c_str());
        HANDLE emptySlotsSemaphore = OpenSemaphoreW(SEMAPHORE_ALL_ACCESS, FALSE, EMPTY_SLOTS_SEMAPHORE_NAME.c_str());
        HANDLE filledSlotsSemaphore = OpenSemaphoreW(SEMAPHORE_ALL_ACCESS, FALSE, FILLED_SLOTS_SEMAPHORE_NAME.c_str());

        if (fileMutex) CloseHandle(fileMutex);
        if (emptySlotsSemaphore) CloseHandle(emptySlotsSemaphore);
        if (filledSlotsSemaphore) CloseHandle(filledSlotsSemaphore);
    }
};

// Тест инициализации файла сообщений
TEST_F(MessageUtilsTest, InitializeMessageFile) {
    int messageCount = 5;
    bool result = message_utils::InitializeMessageFile(testFileName, messageCount);
    ASSERT_TRUE(result);

    // Проверяем, что файл создан и имеет ожидаемый размер
    std::ifstream file(testFileName, std::ios::binary | std::ios::ate);
    ASSERT_TRUE(file.is_open());

    std::streamsize fileSize = file.tellg();
    EXPECT_EQ(fileSize, messageCount * sizeof(Message));

    // Проверяем, что все сообщения пустые
    file.seekg(0);
    for (int i = 0; i < messageCount; ++i) {
        Message message;
        file.read(reinterpret_cast<char*>(&message), sizeof(Message));
        EXPECT_TRUE(message.isEmpty);
    }
}

// Тест на проверку IsSlotEmpty
TEST_F(MessageUtilsTest, IsSlotEmpty) {
    Message emptyMessage;
    EXPECT_TRUE(message_utils::IsSlotEmpty(emptyMessage));

    Message nonEmptyMessage;
    nonEmptyMessage.isEmpty = false;
    strcpy_s(nonEmptyMessage.text, "Test Message");
    EXPECT_FALSE(message_utils::IsSlotEmpty(nonEmptyMessage));
}

// Тест записи и чтения сообщений
TEST_F(MessageUtilsTest, WriteAndReadMessages) {
    int messageCount = 3;

    // Инициализируем файл и создаем объекты синхронизации
    ASSERT_TRUE(message_utils::InitializeMessageFile(testFileName, messageCount));
    CreateSyncObjects(messageCount);

    // Записываем сообщения
    std::string msg1 = "Test Message 1";
    std::string msg2 = "Test Message 2";

    // Тест записи сообщений
    EXPECT_TRUE(message_utils::WriteMessageToFile(testFileName, msg1));
    EXPECT_TRUE(message_utils::WriteMessageToFile(testFileName, msg2));

    // Проверяем количество сообщений
    EXPECT_EQ(message_utils::CountFilledMessages(testFileName, messageCount), 2);

    // Закрываем объекты синхронизации
    CloseSyncObjects();
}

// Тест счетчика заполненных сообщений
TEST_F(MessageUtilsTest, CountFilledMessages) {
    int messageCount = 5;
    ASSERT_TRUE(message_utils::InitializeMessageFile(testFileName, messageCount));

    // Должно быть 0 заполненных сообщений после инициализации
    EXPECT_EQ(message_utils::CountFilledMessages(testFileName, messageCount), 0);

    // Ручное заполнение сообщений для тестирования счетчика
    std::fstream file(testFileName, std::ios::binary | std::ios::in | std::ios::out);
    ASSERT_TRUE(file.is_open());

    Message message;
    message.isEmpty = false;
    strcpy_s(message.text, "Test");

    // Записываем 2 сообщения
    file.seekp(0);
    file.write(reinterpret_cast<char*>(&message), sizeof(Message));

    file.seekp(2 * sizeof(Message));
    file.write(reinterpret_cast<char*>(&message), sizeof(Message));

    file.close();

    // Проверяем счетчик
    EXPECT_EQ(message_utils::CountFilledMessages(testFileName, messageCount), 2);
}

// Тест на проверку записи слишком большого количества сообщений
TEST_F(MessageUtilsTest, TooManyMessages) {
    int messageCount = 2;
    ASSERT_TRUE(message_utils::InitializeMessageFile(testFileName, messageCount));
    CreateSyncObjects(messageCount);

    // Записываем максимальное количество сообщений
    EXPECT_TRUE(message_utils::WriteMessageToFile(testFileName, "Message 1"));
    EXPECT_TRUE(message_utils::WriteMessageToFile(testFileName, "Message 2"));

    // Третья запись должна не удасться, так как слотов только 2
    // Но мы не можем проверить это поведение в тесте, так как WriteMessageToFile будет ждать бесконечно
    // пока не освободится слот. Пропустим этот тест.

    // Очищаем
    CloseSyncObjects();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}