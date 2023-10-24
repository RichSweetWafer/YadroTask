#include "ArgReader.h"
#include "Solution.h"
#include <cstdlib>

int main(int argc, char* argv[])
{
	try
	{
		ArgReader args(argc, argv);
		if (args.callHelp)
			return 0;
		if (args.generateFile != "") {
			std::srand(std::time(nullptr));
			std::vector<int32_t> vec;
			for (size_t i = 0; i < args.generateSize; i++)
				vec.push_back(std::rand() % 200);
			TapeEmulator::createFile(args.generateFile, vec);
			std::cout << "File generated: " << args.generateFile << "\nTape size: " << args.generateSize << "\n";
			vec = TapeEmulator::readFile(args.generateFile);
			for (auto& v : vec)
				std::cout << v << ", ";
			std::cout << std::endl;
		}
		else {
			Solution solution(args.inputFile, args.outputFile);
			solution.solve();

			std::cout << "Input tape: " << args.inputFile << "\n";
			std::vector<int32_t> vec = TapeEmulator::readFile(args.inputFile);
			for (auto& v : vec)
				std::cout << v << ", ";
			std::cout << std::endl;

			std::cout << "Output tape: " << args.outputFile << "\n";
			vec = TapeEmulator::readFile(args.outputFile);
			for (auto& v : vec)
				std::cout << v << ", ";
			std::cout << std::endl;
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	return 0;
}

