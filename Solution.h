#pragma once
#include "Config.h"
#include "TapeEmulator.h"
#include <algorithm>

/**
 * @brief Solution class for the given task. 
*/

class Solution
{
	Config conf;
	TapeEmulator input;
	TapeEmulator output;
	int32_t tapeSize;
	std::filesystem::path dir;
	milliseconds statistics;

public:
	Solution(std::filesystem::path inputFile, std::filesystem::path outputFile);
	
	void solve();
};

