#include "Config.h"

Config::Config() :
	readTime(0), writeTime(0), shiftTime(0), mode(TapeEmulator::Disabled)
{
	std::ifstream config(cfgFile);
	if (config) {
		size_t val;
		if (config >> val)
			readTime = milliseconds(val);
		
		if (config >> val)
			writeTime = milliseconds(val);

		if (config >> val)
			shiftTime = milliseconds(val);

		if (config >> val) {
			switch (val)
			{
			case 2:
				mode = TapeEmulator::Full;
				break;
			case 1:
				mode = TapeEmulator::EmulateTimeStatistics;
				break;
			}
		}
	}
}
