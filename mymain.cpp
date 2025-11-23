//
// Created by Jared Wasserman on 11/22/25.
//

#include <algorithm>
#include <iostream>
#include <string>

#include "bresenham.h"

int main(const int argc, char** argv)
{
	static const std::unordered_map<std::string, std::function<int()>> FUNCTIONS = {
		{"bresenham", bresenham}
	};

	if (argc != 2)
	{
		std::cout << "Must provide exactly 1 argument." << std::endl;
		return 1;
	}

	std::string funcName{argv[1]};
	std::ranges::transform(funcName, funcName.begin(), ::tolower);

	for (const auto& pair : FUNCTIONS)
	{
		if (pair.first == funcName)
		{
			std::cout << "Running function \"" << funcName << "\"." << std::endl;
			return pair.second();
		}
	}

	std::cout << "Invalid argument \"" << funcName << "\"." << std::endl;
	return 1;
}
