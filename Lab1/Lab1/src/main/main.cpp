#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>
#include "header.h"

void PrintBinaryFile(const char* filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Cannot open file for reading\n";
        return;
    }

    employee emp;
    while (file.read(reinterpret_cast<char*>(&emp), sizeof(employee))) {
        std::cout << "ID: " << emp.num << ", Name: " << emp.name
            << ", Hours: " << emp.hours << "\n";
    }
}

void PrintTextFile(const char* filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Cannot open file for reading\n";
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::cout << line << "\n";
    }
}

int main() {
    std::string binary_filename;
    int record_count;

    std::cout << "Enter binary filename: ";
    std::cin >> binary_filename;
    std::cout << "Enter number of records: ";
    std::cin >> record_count;

    std::string creator_cmd = "Creator.exe " + binary_filename + " " +
        std::to_string(record_count);
    STARTUPINFO si = { sizeof(STARTUPINFO) };
    PROCESS_INFORMATION pi;

    if (!CreateProcess(NULL, const_cast<LPSTR>(creator_cmd.c_str()),
        NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        std::cerr << "Failed to launch Creator\n";
        return 1;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    std::cout << "\nBinary file contents:\n";
    PrintBinaryFile(binary_filename.c_str());

    std::string report_filename;
    double hourly_rate;

    std::cout << "\nEnter report filename: ";
    std::cin >> report_filename;
    std::cout << "Enter hourly rate: ";
    std::cin >> hourly_rate;
    
    std::string reporter_cmd = "Reporter.exe " + binary_filename + " " +
        report_filename + " " + std::to_string(hourly_rate);

    if (!CreateProcess(NULL, const_cast<LPSTR>(reporter_cmd.c_str()),
        NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        std::cerr << "Failed to launch Reporter\n";
        return 1;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    std::cout << "\nReport contents:\n";
    PrintTextFile(report_filename.c_str());

    return 0;
}