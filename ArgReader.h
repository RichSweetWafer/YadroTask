#pragma once
#include <filesystem>
#include <iostream>
#include <stdexcept>

namespace fs = std::filesystem;


/**
 * @brief A CLI argument reader, stores given options in itself.
*/

class ArgReader
{
public:
	ArgReader(int argc, char* argv[]);
	
	fs::path inputFile;
	fs::path outputFile;
	
	fs::path generateFile;
	size_t generateSize;
	
	bool callHelp;

};

