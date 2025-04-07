#include "main.h"

void printVec(vector<int>& vec)
{
	for (int i = 0; i < vec.size(); i++)
	{
		cout << vec[i];
	}
	cout << "\n";
}

DWORD WINAPI mark(LPVOID DATA)
{
	Mark* data = (Mark*)DATA;
	
	
	return 0;
}


int main()
{
	int vecSize = 0;
	int markCount = 0;

	cout << "Type size of vector: ";
	cin >> vecSize;
	vector<int> vec(vecSize, 0);

	cout << "Type count of marks: ";
	cin >> markCount;
	HANDLE* hThreads = new HANDLE[markCount];

	for (int i = 0; i < markCount; i++)
	{
		Mark* m = new Mark(i, vec);
		HANDLE hThread = CreateThread(NULL, 0, mark, m, 0, NULL);
		if (hThread == NULL) return GetLastError();
		hThreads[i] = hThread;
	}

	if (WaitForMultipleObjects(markCount, hThreads, TRUE, INFINITE) == WAIT_FAILED)
	{
		cout << "\n\nfailed\n";
	}

	printVec(vec);

	delete[] hThreads;

	return 0;
}