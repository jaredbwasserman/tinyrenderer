//
// Created by Jared Wasserman on 11/22/25.
//

#include <algorithm>
#include <iostream>
#include <ranges>
#include <string>

#include "bresenham.h"

static const std::unordered_map<std::string, std::function<int(const std::vector<std::string>&)>> FUNCTIONS = {
	{"bresenham", Bresenham},
	{"bresenhamhw", BresenhamHW}
};

static void PrintFunctions()
{
	std::cerr << "Select one of these function names:" << std::endl;
	for (const auto& key : FUNCTIONS | std::views::keys)
	{
		std::cerr << "\t" << key << std::endl;
	}
}

int main(const int argc, char** argv)
{
	if (argc < 2)
	{
		std::cerr << "Must provide a function name." << std::endl;
		PrintFunctions();
		return 1;
	}

	std::string funcName{argv[1]};
	std::ranges::transform(funcName, funcName.begin(), ::tolower);

	// Rest of arguments
	std::vector<std::string> arguments;
	for (int i = 2; i <= argc - 1; ++i)
	{
		arguments.emplace_back(argv[i]);
	}

	for (const auto& [key, val] : FUNCTIONS)
	{
		if (key == funcName)
		{
			std::cout << "Running function \"" << funcName << "\"." << std::endl;
			return val(arguments);
		}
	}

	std::cerr << "Invalid function \"" << funcName << "\"." << std::endl;
	PrintFunctions();

	return 1;
}
