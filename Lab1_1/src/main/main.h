#ifndef MAIN_H
#define MAIN_H

#include "../header/header.h"

void displayBinaryFile(const std::string& filename, std::ostream& output = std::cout);
void displayTextFile(const std::string& filename, std::ostream& output = std::cout);
bool startProcess(const std::string& commandLine);

#endif