//
// Created by Jared Wasserman on 11/22/25.
//

#include <algorithm>
#include <iostream>
#include <ranges>
#include <string>

#include "bresenham.h"

static const std::unordered_map<std::string, std::function<int()>> FUNCTIONS = {
	{"bresenham", Bresenham}, {"bresenhamhw", BresenhamHW}
};

static void PrintFunctions()
{
	std::cout << "Select one of these functions:" << std::endl;
	for (const auto& key : FUNCTIONS | std::views::keys)
	{
		std::cout << "\t" << key << std::endl;
	}
}

int main(const int argc, char** argv)
{
	if (argc != 2)
	{
		std::cout << "Must provide a function." << std::endl;
		PrintFunctions();
		return 1;
	}

	std::string funcName{argv[1]};
	std::ranges::transform(funcName, funcName.begin(), ::tolower);

	for (const auto& [key, val] : FUNCTIONS)
	{
		if (key == funcName)
		{
			std::cout << "Running function \"" << funcName << "\"." << std::endl;
			return val();
		}
	}

	std::cout << "Invalid function \"" << funcName << "\"." << std::endl;
	PrintFunctions();

	return 1;
}
