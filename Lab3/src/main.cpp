#include "main.h"

HANDLE consoleMutex;

void printVec(vector<int>& vec)
{
	WaitForSingleObject(consoleMutex, INFINITE);
	for (int i = 0; i < vec.size(); i++)
	{
		cout << vec[i] << " ";
	}
	cout << "\n";
	ReleaseMutex(consoleMutex);
}

DWORD WINAPI mark(LPVOID DATA)
{
	Mark* data = (Mark*)DATA;
	HANDLE events[2] = { data->startEvent, data->stopEvent };

	srand(data->ind + time(0));

	while (true)
	{
		DWORD waitResult = WaitForMultipleObjects(2, events, FALSE, INFINITE);
		int countMarked = 0;
		if(waitResult == WAIT_OBJECT_0) // startEvent
		{
			while (true)
			{
				int randomIndex = rand() % data->vec.size();

				/*WaitForSingleObject(consoleMutex, INFINITE);
				cout << "\nMarker " << data->ind << " | " << randomIndex << "\n";
				ReleaseMutex(consoleMutex);*/

				if (data->vec[randomIndex] == 0)
				{
					Sleep(5);
					data->vec[randomIndex] = data->ind;
					countMarked++;
					Sleep(5);
				}
				else
				{
					WaitForSingleObject(consoleMutex, INFINITE);
					cout << "\nMarker " << data->ind << ": ";
					cout << " | Marked elements: " << countMarked;
					cout << " | Cannot mark element at index: " << randomIndex << "\n";
					ReleaseMutex(consoleMutex);

					SetEvent(data->noMoreWorkEvent);
					break;
				}
			}
		}
		else if (waitResult == WAIT_OBJECT_0 + 1) // stopEvent
		{ 
			for (int i = 0; i < data->vec.size(); i++)
			{
				if (data->vec[i] == data->ind)
				{
					data->vec[i] = 0;
				}
			}
			return 0;
		}
	}
	
	return 0;
}


int main()
{
	int vecSize = 0;
	int markCount = 0;

	consoleMutex = CreateMutex(NULL, FALSE, NULL);
	if (consoleMutex == NULL) 
	{
		cout << "Failed to create console mutex\n";
		return GetLastError();
	}

	cout << "Type size of vector: ";
	cin >> vecSize;
	vector<int> vec(vecSize, 0);

	cout << "Type count of marks: ";
	cin >> markCount;

	vector<HANDLE> hThreads(markCount);
	vector<HANDLE> startEvents(markCount);
	vector<HANDLE> stopEvents(markCount);
	vector<HANDLE> noMoreWorkEvents(markCount);
	vector<unique_ptr<Mark>> marks(markCount);

	for (int i = 0; i < markCount; i++)
	{
		startEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		stopEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		noMoreWorkEvents[i] = CreateEvent(NULL, TRUE, FALSE, NULL);

		marks[i] = make_unique<Mark>(i + 1, vec, startEvents[i], stopEvents[i], noMoreWorkEvents[i]);
		hThreads[i] = CreateThread(NULL, 0, mark, marks[i].get(), 0, NULL);

		if (hThreads[i] == NULL) 
		{
			cout << "Failed to create thread\n";
			return GetLastError();
		}
	}

	while (true)
	{
		for (int i = 0; i < markCount; i++)
		{
			if (hThreads[i] != NULL)
			{
				ResetEvent(noMoreWorkEvents[i]);
			}
		}

		for (int i = 0; i < markCount; i++) 
		{
			if (hThreads[i] != NULL) 
			{
				SetEvent(startEvents[i]);
			}
		}

		WaitForMultipleObjects(markCount, noMoreWorkEvents.data(), TRUE, INFINITE);

		cout << "\nCurrent array state: ";
		printVec(vec);

		int threadToStop;
		
		while (true)
		{
			cout << "\nEnter thread number to stop (1-" << markCount << "): ";
			cin >> threadToStop;

			if (threadToStop < 1 || threadToStop > markCount)
			{
				cout << "Invalid thread number\n";
				continue;
			}

			if (hThreads[threadToStop - 1] == NULL)
			{
				cout << "This thread is already stopped\n";
				continue;
			}
			break;
		}
		

		SetEvent(stopEvents[threadToStop - 1]);

		WaitForSingleObject(hThreads[threadToStop - 1], INFINITE);
		CloseHandle(hThreads[threadToStop - 1]);
		hThreads[threadToStop - 1] = NULL;

		cout << "\nArray state after stopping thread " << threadToStop << ": ";
		printVec(vec);

		bool allThreadsStopped = true;
		for (int i = 0; i < markCount; i++)
		{
			if (hThreads[i] != NULL)
			{
				allThreadsStopped = false;
				break;
			}
		}

		if (allThreadsStopped) 
		{
			break;
		}
	}

	for (int i = 0; i < markCount; i++) 
	{
		CloseHandle(startEvents[i]);
		CloseHandle(stopEvents[i]);
		CloseHandle(noMoreWorkEvents[i]);
	}

	CloseHandle(consoleMutex);

	return 0;
}