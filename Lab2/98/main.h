#pragma once
#include <iostream>
#include <vector>
#include <windows.h>

struct Avg 
{
    std::vector<int> vec;
    int avg;
};

struct MinMax 
{
    std::vector<int> vec;
    int min;
    int max;
};

void printVec(std::vector<int>& vec);
DWORD WINAPI average(LPVOID data);
DWORD WINAPI min_max(LPVOID data);