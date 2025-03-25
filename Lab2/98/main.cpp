#include "main.h"

void printVec(std::vector<int>& vec) 
{
    for (size_t i = 0; i < vec.size(); i++) 
    {
        std::cout << vec[i] << " ";
    }
    std::cout << "\n";
}

DWORD WINAPI average(LPVOID data) 
{
    Avg* _avg = (Avg*)data;
    _avg->avg = 0;
    if (_avg->vec.empty()) return 0;

    for (size_t i = 0; i < _avg->vec.size(); i++) 
    {
        _avg->avg += _avg->vec[i];
        Sleep(12);
    }
    _avg->avg /= static_cast<int>(_avg->vec.size());
    std::cout << "Avg: " << _avg->avg << "\n";
    return 0;
}

DWORD WINAPI min_max(LPVOID data) 
{
    MinMax* _min_max = (MinMax*)data;
    if (_min_max->vec.empty()) return 0;

    _min_max->min = _min_max->vec[0];
    _min_max->max = _min_max->vec[0];
    for (size_t i = 1; i < _min_max->vec.size(); i++) 
    {
        _min_max->min = min(_min_max->min, _min_max->vec[i]);
        _min_max->max = max(_min_max->max, _min_max->vec[i]);
        Sleep(7);
    }
    std::cout << "Min: " << _min_max->min << " Max: " << _min_max->max << "\n";
    return 0;
}

#ifndef TESTING
int main() 
{
    int n;
    std::cout << "Type number of elements:";
    std::cin >> n;
    std::vector<int> vec(n, 0);
    std::cout << "Type elements: ";
    for (int i = 0; i < n; i++) 
    {
        std::cin >> vec[i];
    }

    Avg avg;
    avg.vec = vec;

    MinMax minMax;
    minMax.vec = vec;

    HANDLE hThreadAvg = CreateThread(NULL, 0, average, &avg, 0, NULL);
    if (hThreadAvg == NULL) return GetLastError();

    HANDLE hThreadMin_Max = CreateThread(NULL, 0, min_max, &minMax, 0, NULL);
    if (hThreadMin_Max == NULL) return GetLastError();

    WaitForSingleObject(hThreadAvg, INFINITE);
    CloseHandle(hThreadAvg);

    WaitForSingleObject(hThreadMin_Max, INFINITE);
    CloseHandle(hThreadMin_Max);

    return 0;
}
#endif