#include <gtest/gtest.h>
#include "../src/creator/creator.h"
#include <fstream>
#include <vector>

class CreatorTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Создаем временное имя файла для тестов
        testFilename = "test_employees.bin";
    }

    void TearDown() override {
        // Удаляем тестовый файл после каждого теста
        std::remove(testFilename.c_str());
    }

    std::string testFilename;
};

// Тест на создание файла с отрицательным количеством записей
TEST_F(CreatorTest, NegativeRecordCount) {
    EXPECT_FALSE(createEmployeeFile(testFilename, -1));
}

// Тест на создание файла с нулевым количеством записей
TEST_F(CreatorTest, ZeroRecordCount) {
    EXPECT_FALSE(createEmployeeFile(testFilename, 0));
}

// Тест на создание файла с предопределенными данными
TEST_F(CreatorTest, CreateFileWithPredefinedData) {
    std::vector<employee> employees = {
        {1, "John", 40},
        {2, "Alice", 35},
        {3, "Bob", 45}
    };

    EXPECT_TRUE(createEmployeeFile(testFilename, 3, employees));

    // Проверяем, что файл создан и содержит корректные данные
    std::ifstream file(testFilename, std::ios::binary);
    ASSERT_TRUE(file.is_open());

    employee emp;
    int count = 0;
    while (file.read(reinterpret_cast<char*>(&emp), sizeof(employee))) {
        EXPECT_EQ(emp.num, employees[count].num);
        EXPECT_STREQ(emp.name, employees[count].name);
        EXPECT_EQ(emp.hours, employees[count].hours);
        count++;
    }

    EXPECT_EQ(count, 3);
    file.close();
}

// Тест на попытку создания файла с некорректным путем
TEST_F(CreatorTest, InvalidFilePath) {
    EXPECT_FALSE(createEmployeeFile("/invalid/path/file.bin", 1));
}

// Тест на создание файла с максимально допустимым количеством записей
TEST_F(CreatorTest, MaxRecordCount) {
    std::vector<employee> employees(100, { 1, "Test", 40 });  // Создаем 100 записей
    EXPECT_TRUE(createEmployeeFile(testFilename, 100, employees));

    std::ifstream file(testFilename, std::ios::binary);
    ASSERT_TRUE(file.is_open());

    employee emp;
    int count = 0;
    while (file.read(reinterpret_cast<char*>(&emp), sizeof(employee))) {
        count++;
    }

    EXPECT_EQ(count, 100);
    file.close();
}

// Тест на проверку размера созданного файла
TEST_F(CreatorTest, CheckFileSize) {
    std::vector<employee> employees = {
        {1, "John", 40},
        {2, "Alice", 35}
    };

    EXPECT_TRUE(createEmployeeFile(testFilename, 2, employees));

    std::ifstream file(testFilename, std::ios::binary | std::ios::ate);
    ASSERT_TRUE(file.is_open());

    EXPECT_EQ(file.tellg(), 2 * sizeof(employee));
    file.close();
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}