#ifndef header_main
#define header_main

#include <iostream>
#include <vector>
#include <windows.h>
using namespace std;

struct Mark
{
	vector<int>& vec;
	int ind;
	Mark(int ind, vector<int>& vec) : ind(ind), vec(vec) {}
};

#endif // !header_main
