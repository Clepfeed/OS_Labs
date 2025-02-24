#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include "header.h"

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: Reporter <binary_file> <report_file> <hourly_rate>\n";
        return 1;
    }

    const char* binary_file = argv[1];
    const char* report_file = argv[2];
    double hourly_rate = std::atof(argv[3]);

    std::ifstream bin_file(binary_file, std::ios::binary);
    if (!bin_file) {
        std::cerr << "Cannot open binary file\n";
        return 1;
    }

    std::ofstream report(report_file);
    if (!report) {
        std::cerr << "Cannot create report file\n";
        return 1;
    }

    std::vector<employee> employees;
    employee emp;
    while (bin_file.read(reinterpret_cast<char*>(&emp), sizeof(employee))) {
        employees.push_back(emp);
    }

    report << "Отчет по файлу \"" << binary_file << "\"\n";
    report << "Номер сотрудника\tИмя сотрудника\tЧасы\tЗарплата\n";

    for (const auto& e : employees) {
        double salary = e.hours * hourly_rate;
        report << e.num << "\t\t" << e.name << "\t\t"
            << e.hours << "\t" << salary << "\n";
    }

    return 0;
}