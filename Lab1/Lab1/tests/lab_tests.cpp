#include <gtest/gtest.h>
#include <Windows.h>
#include <fstream>
#include <string>
#include "header.h"

class LabTest : public ::testing::Test {
protected:
    void SetUp() override {
        remove("test.bin");
        remove("test_report.txt");
    }

    void TearDown() override {
        remove("test.bin");
        remove("test_report.txt");
    }

    void CreateTestBinaryFile(const char* filename, const std::vector<employee>& employees) {
        std::ofstream file(filename, std::ios::binary);
        for (const auto& emp : employees) {
            file.write(reinterpret_cast<const char*>(&emp), sizeof(employee));
        }
    }
};

// Тест создания процесса Creator
TEST_F(LabTest, CreatorProcessTest) {
    STARTUPINFO si = { sizeof(STARTUPINFO) };
    PROCESS_INFORMATION pi;
    std::string cmd = "Creator.exe test.bin 1";

    BOOL result = CreateProcess(
        NULL,
        const_cast<LPSTR>(cmd.c_str()),
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &si,
        &pi
    );

    ASSERT_TRUE(result) << "Failed to create Creator process";

    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exitCode;
    GetExitCodeProcess(pi.hProcess, &exitCode);
    EXPECT_EQ(exitCode, 0);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

// Тест создания процесса Reporter
TEST_F(LabTest, ReporterProcessTest) {
    std::vector<employee> test_data = {
        {1, "Test1", 40.0},
        {2, "Test2", 35.5}
    };
    CreateTestBinaryFile("test.bin", test_data);

    STARTUPINFO si = { sizeof(STARTUPINFO) };
    PROCESS_INFORMATION pi;
    std::string cmd = "Reporter.exe test.bin test_report.txt 100.0";

    BOOL result = CreateProcess(
        NULL,
        const_cast<LPSTR>(cmd.c_str()),
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &si,
        &pi
    );

    ASSERT_TRUE(result) << "Failed to create Reporter process";

    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exitCode;
    GetExitCodeProcess(pi.hProcess, &exitCode);
    EXPECT_EQ(exitCode, 0);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    std::ifstream report("test_report.txt");
    ASSERT_TRUE(report.is_open());
}

// Тест полного рабочего процесса
TEST_F(LabTest, FullWorkflowTest) {
    std::vector<employee> test_employees = {
        {1, "John", 40.0},
        {2, "Alice", 35.5},
        {3, "Bob", 45.0}
    };
    CreateTestBinaryFile("test.bin", test_employees);

    // Запускаем Reporter
    STARTUPINFO si = { sizeof(STARTUPINFO) };
    PROCESS_INFORMATION pi;
    std::string reporter_cmd = "Reporter.exe test.bin test_report.txt 100.0";

    BOOL result = CreateProcess(
        NULL,
        const_cast<LPSTR>(reporter_cmd.c_str()),
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &si,
        &pi
    );

    ASSERT_TRUE(result);
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    // Проверяем содержимое отчета
    std::ifstream report("test_report.txt");
    ASSERT_TRUE(report.is_open());

    std::string line;
    std::vector<std::string> lines;
    while (std::getline(report, line)) {
        lines.push_back(line);
    }

    EXPECT_GE(lines.size(), 4); // Заголовок + 3 сотрудника
}

// Тест обработки ошибок
TEST_F(LabTest, ErrorHandlingTest) {
    STARTUPINFO si = { sizeof(STARTUPINFO) };
    PROCESS_INFORMATION pi;

    // Тест с несуществующим файлом
    std::string cmd = "Reporter.exe nonexistent.bin test_report.txt 100.0";

    BOOL result = CreateProcess(
        NULL,
        const_cast<LPSTR>(cmd.c_str()),
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &si,
        &pi
    );

    ASSERT_TRUE(result);
    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exitCode;
    GetExitCodeProcess(pi.hProcess, &exitCode);
    EXPECT_NE(exitCode, 0); // Ожидаем ненулевой код возврата при ошибке

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}