// creator.cpp

#include "../header/header.h"

int main(int argc, char* argv[]) 
{
    if (argc != 3) 
    {
        std::cerr << "Usage: Creator <filename> <number_of_records>\n";
        return 1;
    }

    std::string filename = argv[1];
    int recordCount;

    try 
    {
        recordCount = std::stoi(argv[2]);
    }
    catch (const std::exception& e) 
    {
        std::cerr << "Error: Invalid number of records\n";
        return 1;
    }

    if (recordCount <= 0) 
    {
        std::cerr << "Error: Number of records should be > 0\n";
        return 1;
    }

    std::ofstream file(filename, std::ios::binary);
    if (!file) 
    {
        std::cerr << "Error: Cannot open file " << filename << " for writing\n";
        return 1;
    }

    employee emp;
    std::cout << "Enter information for " << recordCount << " employees:\n";

    for (int i = 0; i < recordCount; i++) 
    {
        std::cout << "Employee #" << (i + 1) << "\n";

        std::cout << "ID: ";
        std::cin >> emp.num;

        std::cout << "Name: ";
        std::cin >> emp.name;

        std::cout << "Hours: ";
        std::cin >> emp.hours;

        file.write(reinterpret_cast<char*>(&emp), sizeof(employee));

        if (!file) 
        {
            std::cerr << "Error writing to file\n";
            file.close();
            return 1;
        }
    }

    file.close();
    std::cout << "Binary file " << filename << " created successfully with " << recordCount << " records.\n";

    return 0;
}