#include "main.h"

void displayBinaryFile(const std::string& filename, std::ostream& output) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        output << "Error: Cannot open file " << filename << " for reading\n";
        return;
    }

    employee emp;
    int counter = 0;

    output << "\nContents of binary file " << filename << ":\n";
    output << "ID|Name|Hours\n";
    output << "------------------------\n";

    while (file.read(reinterpret_cast<char*>(&emp), sizeof(employee))) {
        output << emp.num << "|" << emp.name << "|" << emp.hours << "\n";
        counter++;
    }

    file.close();
    output << "Total records: " << counter << "\n\n";
}

void displayTextFile(const std::string& filename, std::ostream& output) {
    std::ifstream file(filename);
    if (!file) {
        output << "Error: Cannot open file " << filename << " for reading\n";
        return;
    }

    output << "\nContents of report file " << filename << ":\n";
    output << "------------------------\n";

    std::string line;
    while (getline(file, line)) {
        output << line << "\n";
    }

    file.close();
    output << "\n";
}

bool startProcess(const std::string& commandLine) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    char* cmdLine = _strdup(commandLine.c_str());

    if (!CreateProcess(NULL, cmdLine, NULL, NULL,
        FALSE, 0, NULL, NULL, &si, &pi)) {
        std::cerr << "CreateProcess failed: " << GetLastError() << std::endl;
        free(cmdLine);
        return false;
    }

    free(cmdLine);

    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exitCode;
    GetExitCodeProcess(pi.hProcess, &exitCode);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return (exitCode == 0);
}

#ifndef TESTING
int main() {
    std::string creatorName = "creator.exe";
    std::string reporterName = "reporter.exe";
    std::string creatorFileName = "";
    std::string reporterFileName = "";
    std::string recordCount = "";
    std::string salaryPerHour = "";

    std::cout << "Type data for creator:\n"
        << "filename: ";
    std::cin >> creatorFileName;
    std::cout << "records count: ";
    std::cin >> recordCount;

    creatorName += " " + creatorFileName + " " + recordCount;

    if (!startProcess(creatorName)) {
        std::cerr << "Creator utility failed\n";
        return 1;
    }

    displayBinaryFile(creatorFileName);

    std::cout << "Type data for reporter:\n" << "reporter filename: ";
    std::cin >> reporterFileName;
    std::cout << "salary per hour: ";
    std::cin >> salaryPerHour;

    reporterName += " " + creatorFileName + " " + reporterFileName + ".txt " + salaryPerHour;

    if (!startProcess(reporterName)) {
        std::cerr << "Reporter utility failed\n";
        return 1;
    }

    displayTextFile(reporterFileName + ".txt");

    return 0;
}
#endif