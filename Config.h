#pragma once
#include "TapeEmulator.h"
/**
 * @brief Configuration object. Reads parameters on object initialization from a file with given constant path.
 * Config structure:
 * 1st number - read time in milliseconds;
 * 2nd number - write time in milliseconds;
 * 3rd number - shift time in milliseconds;
 * 4th number - emulation mode: 0 (default) - no emulation; 1 - partial emulation; 2 - full emulation.
*/
class Config
{
public:
	const std::filesystem::path cfgFile = "Yadro.cfg";
	Config();
	milliseconds readTime;
	milliseconds writeTime;
	milliseconds shiftTime;

	TapeEmulator::Emulation mode;
};

