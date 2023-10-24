#include "Solution.h"

/**
 * @brief Constucts solution object by initializing tapes.
 * @param inputFile - input tape file path
 * @param outputFile - sorted tape file path
*/

Solution::Solution(std::filesystem::path inputFile, std::filesystem::path outputFile)
	: dir("tmp"), statistics(0)
{
	if (!input.initialize(conf.mode, inputFile, conf.readTime, conf.writeTime, conf.shiftTime))
		throw std::runtime_error("Input file did not initialize.");
	tapeSize = input.size();
		
	std::ofstream out(outputFile, std::fstream::binary | std::fstream::trunc);
	out.write(reinterpret_cast<char*>(&tapeSize), sizeof(int32_t));
	out.close();

	if (!output.initialize(conf.mode, outputFile, conf.readTime, conf.writeTime, conf.shiftTime))
		throw std::runtime_error("Output file did not initialize.");	
}


/**
 * @brief sort the input tape using temporary tapes and put them together into the output tape.
*/
void Solution::solve()
{
	
	std::filesystem::create_directory(dir);
	std::vector<std::filesystem::path> tmpPaths;
	
	//// separate input tape into n temporary parts
	
	const size_t parts = 3;
	for (size_t i = 0; i < parts; i++) {
		// push values to "RAM"
		std::vector<int32_t> ram;
		size_t base = i * size_t(tapeSize / 3);
		size_t border = base + size_t(tapeSize / 3);
		if (i == parts - 1)
			border = input.size();

		for (size_t j = base; j < border; j++)
		{
			ram.push_back(input.read());
			if (j != input.size() - 1)
				input.shiftForward();
		}

		// sort pushed values
		std::sort(ram.begin(), ram.end());

		// write values to the corresponding tape file
		std::filesystem::path tmpPath = dir / ("file" + std::to_string(i) + ".tape");

		std::ofstream tmpTape(tmpPath, std::fstream::binary);
		
		if (tmpTape) {
			int32_t ramSize = ram.size();
			tmpTape.write(reinterpret_cast<char*>(&ramSize), sizeof(int32_t));
			for (auto val : ram) {
				tmpTape.write(reinterpret_cast<char*>(&val), sizeof(int32_t));
			}
			tmpTape.close();
			tmpPaths.push_back(tmpPath);
		}
		else
			throw std::runtime_error("Could not open temporary tape file.");
	}

	//// Combine sorted tapes to a single one

	// Tapes initialization
	// TapeEmulator is neither movable nor copyable, so TapeEmulator* pointer is used
	std::vector<TapeEmulator*> tmpTapes;
	for (size_t i = 0; i < parts; i++)
	{
		tmpTapes.emplace_back();
		tmpTapes[i] = new TapeEmulator();
		if (!tmpTapes[i]->initialize(conf.mode, tmpPaths[i], conf.readTime, conf.writeTime, conf.shiftTime))
			throw std::runtime_error("Could not initialize temporary tape.");
	}

	// select max value for insertion from tapes' currently pointed cells and shift forward
	// the tape with this value
	for (size_t t = 0; t < output.size(); t++) {
		int32_t min = std::numeric_limits<int32_t>::max();
		size_t ind = 0;
		for (size_t i = 0; i < tmpTapes.size(); i++) {
			int32_t val = tmpTapes[i]->read();
			if (val < min) {
				min = val;
				ind = i;
			}
		}
		output.write(min);
		if (t != output.size() - 1)
			output.shiftForward();
		if (tmpTapes[ind]->position() != tmpTapes[ind]->size() - 1)
			tmpTapes[ind]->shiftForward();
		else {
			statistics += tmpTapes[ind]->getElapsed();
			delete tmpTapes[ind];
			tmpTapes.erase(tmpTapes.begin() + ind);
		}
	}

	// Time statistics, if enabled
	if (conf.mode != TapeEmulator::Disabled)
		std::cout << "\nExecution time: " << statistics << '\n';
}
