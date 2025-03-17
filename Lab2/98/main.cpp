#include "main.h"

void printVec(vector<int>& vec)
{
	for (int i = 0; i < vec.size(); i++)
	{
		cout << vec[i] << " ";
	}
	cout << "\n";
}

DWORD WINAPI average(LPVOID data)
{
	Avg* _avg = (Avg*)data;
	_avg->avg = 0;
	for (int i = 0; i < _avg->vec.size(); i++)
	{
		_avg->avg += _avg->vec[i];
		Sleep(12);
	}
	_avg->avg /= _avg->vec.size();
	return 0;
}

int main()
{
	int n;
	cout << "Type number of elements:";
	cin >> n;
	vector<int> vec(n, 0);
	for (int i = 0; i < n; i++)
	{
		cout << "Type element " << i << ": ";
		cin >> vec[i];
	}

	Avg avg;
	avg.avg = 0;
	avg.vec = vec;

	HANDLE hThread;
	DWORD IDThread;

	hThread = CreateThread(NULL, 0, average, &avg, 0, &IDThread);
	if(hThread == NULL) return GetLastError();

	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	cout << avg.avg << "\n";
	

	return 0;
}