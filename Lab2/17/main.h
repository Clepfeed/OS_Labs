#pragma once
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

struct Avg {
    std::vector<int> vec;
    int avg;
};

struct MinMax {
    std::vector<int> vec;
    int min;
    int max;
};

void printVec(std::vector<int>& vec);
void average(Avg* data);
void min_max(MinMax* data);