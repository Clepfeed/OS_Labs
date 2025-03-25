#include "main.h"

void printVec(std::vector<int>& vec) 
{
    for (const auto& num : vec) 
    {
        std::cout << num << " ";
    }
    std::cout << "\n";
}

void average(Avg* data) 
{
    auto& [vec, avg] = *data;
    avg = 0;
    if (vec.empty()) return;

    for (const auto& num : vec) 
    {
        avg += num;
        std::this_thread::sleep_for(std::chrono::milliseconds(12));
    }
    avg /= static_cast<int>(vec.size());
    std::cout << "Avg: " << avg << "\n";
}

void min_max(MinMax* data) 
{
    auto& [vec, min, max] = *data;
    if (vec.empty()) return;

    min = vec[0];
    max = vec[0];
    for (const auto& num : vec) 
    {
        min = std::min(min, num);
        max = std::max(max, num);
        std::this_thread::sleep_for(std::chrono::milliseconds(7));
    }
    std::cout << "Min: " << min << " Max: " << max << "\n";
}

#ifndef TESTING
int main() 
{
    int n;
    std::cout << "Type number of elements:";
    std::cin >> n;
    std::vector<int> vec(n);
    std::cout << "Type elements: ";
    for (auto& num : vec) 
    {
        std::cin >> num;
    }

    Avg avg{ vec, 0 };
    MinMax minMax{ vec, 0, 0 };

    std::thread avgThread(average, &avg);
    avgThread.join();

    std::thread minMaxThread(min_max, &minMax);
    minMaxThread.join();

    return 0;
}
#endif