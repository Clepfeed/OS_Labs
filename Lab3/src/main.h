#ifndef header_main
#define header_main

#include <iostream>
#include <vector>
#include <windows.h>
#include <random>
using namespace std;

struct Mark 
{
    int ind;
    vector<int>& vec;
    HANDLE startEvent;
    HANDLE stopEvent;
    HANDLE noMoreWorkEvent;

    Mark(int index, vector<int>& vector, HANDLE start, HANDLE stop, HANDLE noMore)
        : ind(index), vec(vector), startEvent(start), stopEvent(stop), noMoreWorkEvent(noMore) {}
};

#endif // !header_main
