#include <gtest/gtest.h>
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <windows.h>
#include "header.h"

// ����� ��� Creator
TEST(CreatorTest, CreatesFileWithCorrectData) {
    const char* filename = "test_creator.bin";
    const char* creator_cmd = "Creator.exe test_creator.bin 1";

    // ������ Creator
    STARTUPINFO si = { sizeof(STARTUPINFO) };
    PROCESS_INFORMATION pi;
    if (!CreateProcess(NULL, const_cast<LPSTR>(creator_cmd), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        FAIL() << "Failed to launch Creator";
    }
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    // �������� ����������� �����
    std::ifstream file(filename, std::ios::binary);
    ASSERT_TRUE(file.is_open());

    employee emp;
    file.read(reinterpret_cast<char*>(&emp), sizeof(employee));
    EXPECT_GT(emp.num, 0);
    EXPECT_GT(emp.hours, 0);
    file.close();

    // �������� ��������� �����
    std::remove(filename);
}

// ����� ��� Reporter
TEST(ReporterTest, GeneratesReportWithCorrectData) {
    const char* binary_filename = "test_reporter.bin";
    const char* report_filename = "test_report.txt";
    const char* creator_cmd = "Creator.exe test_reporter.bin 1";
    const char* reporter_cmd = "Reporter.exe test_reporter.bin test_report.txt 10.0";

    // ������ Creator
    STARTUPINFO si = { sizeof(STARTUPINFO) };
    PROCESS_INFORMATION pi;
    if (!CreateProcess(NULL, const_cast<LPSTR>(creator_cmd), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        FAIL() << "Failed to launch Creator";
    }
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    // ������ Reporter
    if (!CreateProcess(NULL, const_cast<LPSTR>(reporter_cmd), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        FAIL() << "Failed to launch Reporter";
    }
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    // �������� ����������� ������
    std::ifstream report(report_filename);
    ASSERT_TRUE(report.is_open());

    std::string line;
    std::getline(report, line); // ���������� ���������
    std::getline(report, line);
    EXPECT_NE(line.find("10"), std::string::npos); // ���������, ��� �������� ���������� ���������
    report.close();

    // �������� �������� ������
    std::remove(binary_filename);
    std::remove(report_filename);
}

// ����� ��� Main
TEST(MainTest, FullWorkflow) {
    const char* binary_filename = "test_main.bin";
    const char* report_filename = "test_main_report.txt";
    const char* creator_cmd = "Creator.exe test_main.bin 1";
    const char* reporter_cmd = "Reporter.exe test_main.bin test_main_report.txt 10.0";

    // ������ Creator
    STARTUPINFO si = { sizeof(STARTUPINFO) };
    PROCESS_INFORMATION pi;
    if (!CreateProcess(NULL, const_cast<LPSTR>(creator_cmd), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        FAIL() << "Failed to launch Creator";
    }
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    // ������ Reporter
    if (!CreateProcess(NULL, const_cast<LPSTR>(reporter_cmd), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        FAIL() << "Failed to launch Reporter";
    }
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    // �������� ����������� ������
    std::ifstream report(report_filename);
    ASSERT_TRUE(report.is_open());

    std::string line;
    std::getline(report, line); // ���������� ���������
    std::getline(report, line);
    EXPECT_NE(line.find("10"), std::string::npos); // ���������, ��� �������� ���������� ���������
    report.close();

    // �������� �������� ������
    std::remove(binary_filename);
    std::remove(report_filename);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}