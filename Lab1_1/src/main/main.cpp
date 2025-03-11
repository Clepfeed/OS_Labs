// main.cpp

#include "../header/header.h"

using namespace std;

void displayBinaryFile(const string& filename) 
{
	ifstream file(filename, ios::binary);
	if (!file) 
	{
		cerr << "Error: Cannot open file " << filename << " for reading\n";
		return;
	}

	employee emp;
	int counter = 0;

	cout << "\nContents of binary file " << filename << ":\n";
	cout << "ID|Name|Hours\n";
	cout << "------------------------\n";

	while (file.read(reinterpret_cast<char*>(&emp), sizeof(employee))) 
	{
		cout << emp.num << "|" << emp.name << "|" << emp.hours << "\n";
		counter++;
	}

	file.close();
	cout << "Total records: " << counter << "\n\n";
}

void displayTextFile(const string& filename) 
{
	ifstream file(filename);
	if (!file) 
	{
		cerr << "Error: Cannot open file " << filename << " for reading\n";
		return;
	}

	cout << "\nContents of report file " << filename << ":\n";
	cout << "------------------------\n";

	string line;
	while (getline(file, line)) 
	{
		cout << line << "\n";
	}

	file.close();
	cout << "\n";
}

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
		cerr << "CreateProcess failed: " << GetLastError() << endl;
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
	string creatorName = "creator.exe";
	string reporterName = "reporter.exe";
	string creatorFileName = "";
	string reporterFileName = "";
	string recordCount = "";
	string salaryPerHour = "";
	
	cout << "Type data for creator:\n"
		 << "filename: ";
	cin >> creatorFileName;
	cout << "records count: ";
	cin >> recordCount;

	creatorName += " " + creatorFileName + " " + recordCount;

	if (!startProcess(creatorName))
	{
		cerr << "Creator utility failed\n";
		return 1;
	}

	displayBinaryFile(creatorFileName);

	cout << "Type data for reporter:\n"
		<< "reporter filename: ";
	cin >> reporterFileName;
	cout << "salary per hour: ";
	cin >> salaryPerHour;

	reporterName += " " + creatorFileName + " " + reporterFileName + ".txt " + salaryPerHour;

	if (!startProcess(reporterName))
	{
		cerr << "Creator utility failed\n";
		return 1;
	}

	displayTextFile(reporterFileName);

	return 0;
}
