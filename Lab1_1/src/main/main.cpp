// main.cpp

#include "../header/header.h"

using namespace std;

bool startProcess(const string& commandLine)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	char* cmdLine = _strdup(commandLine.c_str());

	if (!CreateProcess(NULL, cmdLine, NULL, NULL,
		FALSE, 0, NULL, NULL, &si, &pi))
	{
		std::cerr << "CreateProcess failed: " << GetLastError() << std::endl;
		free(cmdLine);
		return false;
	}

	free(cmdLine);

	WaitForSingleObject(pi.hProcess, INFINITE);

	DWORD exitCode;
	GetExitCodeProcess(pi.hProcess, &exitCode);

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return (exitCode == 0);
}

int main()
{
	string creatorName = "creator.exe 1 2 3";

	cout << "Hello CMake. It's main\n\n";

	/*system("dir *.exe");
	system("pause");*/

	if (!startProcess(creatorName))
	{
		cerr << "Creator utility failed\n";
		return 1;
	}

	return 0;
}
