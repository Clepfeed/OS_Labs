// creator.cpp

#include "../header/header.h"

int main(int argc, char* argv[])
{
	std::cout << "Creator:\n";
	for (int i = 0; i < argc; i++)
	{
		std::cout << argv[i] << "\n";
	}
		
	return 0;
}