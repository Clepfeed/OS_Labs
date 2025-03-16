#include "reporter.h"

std::vector<employee> readEmployeesFromFile(const std::string& filename) {
    std::vector<employee> employees;
    std::ifstream file(filename, std::ios::binary);

    if (!file) {
        return employees;
    }

    employee emp;
    while (file.read(reinterpret_cast<char*>(&emp), sizeof(employee))) {
        employees.push_back(emp);
    }

    file.close();
    return employees;
}

bool generateReport(const std::string& binaryFilename, const std::string& reportFilename, double hourlyRate) {
    if (hourlyRate <= 0) {
        std::cerr << "Error: Hourly rate should be positive" << "\n";
        return false;
    }

    std::vector<employee> employees = readEmployeesFromFile(binaryFilename);

    if (employees.empty()) {
        std::cerr << "Warning: No records found in " << binaryFilename << "\n";
        return false;
    }

    std::sort(employees.begin(), employees.end(),
        [](const employee& a, const employee& b) { return a.hours < b.hours; });

    std::ofstream reportFile(reportFilename);
    if (!reportFile) {
        std::cerr << "Error: Cannot open file " << reportFilename << " for writing" << "\n";
        return false;
    }

    reportFile << "Отчет по файлу \"" << binaryFilename << "\"" << "\n";
    reportFile << "Номер сотрудника|Имя сотрудника|Часы|Зарплата" << "\n";

    reportFile << std::fixed << std::setprecision(3);
    for (const auto& e : employees) {
        double salary = e.hours * hourlyRate;
        reportFile << e.num << "|" << e.name << "|"
            << e.hours << "|" << salary << "\n";
    }

    reportFile.close();
    std::cout << "Report file " << reportFilename << " created successfully." << "\n";
    return true;
}

#ifndef TESTING
int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: Reporter <binary_filename> <report_filename> <hourly_rate>" << "\n";
        return 1;
    }

    std::string binaryFilename = argv[1];
    std::string reportFilename = argv[2];
    double hourlyRate;

    try {
        hourlyRate = std::stod(argv[3]);
    }
    catch (const std::exception& e) {
        std::cerr << "Error: Invalid hourly rate" << "\n";
        return 1;
    }

    if (!generateReport(binaryFilename, reportFilename, hourlyRate)) {
        return 1;
    }

    return 0;
}
#endif