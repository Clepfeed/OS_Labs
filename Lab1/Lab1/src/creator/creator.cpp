#include <windows.h>
#include <iostream>
#include "header.h"

using namespace std;

int main(int argc, char* argv[]) 
{
    if (argc != 3) 
    {
        cout << "Usage: Creator filename number_of_records\n";
        return 1;
    }

    const char* filename = argv[1];
    int n = atoi(argv[2]);

    FILE* file;
    fopen_s(&file, filename, "wb");
    if (!file) 
    {
        cout << "Cannot open file for writing\n";
        return 1;
    }

    employee emp;
    for (int i = 0; i < n; i++) 
    {
        cout << "Enter employee #" << (i + 1) << " data:\n";
        cout << "ID: ";
        cin >> emp.num;
        cout << "Name: ";
        cin >> emp.name;
        cout << "Hours: ";
        cin >> emp.hours;

        fwrite(&emp, sizeof(employee), 1, file);
    }

    fclose(file);
    return 0;
}