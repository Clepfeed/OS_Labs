#ifndef REPORTER_H
#define REPORTER_H

#include "../header/header.h"

std::vector<employee> readEmployeesFromFile(const std::string& filename);
bool generateReport(const std::string& binaryFilename, const std::string& reportFilename, double hourlyRate);

#endif