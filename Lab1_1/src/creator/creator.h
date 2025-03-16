#ifndef CREATOR_H
#define CREATOR_H

#include "../header/header.h"

bool createEmployeeFile(const std::string& filename, int recordCount, const std::vector<employee>& employees = {});

#endif