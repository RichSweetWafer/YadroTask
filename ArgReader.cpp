#include "ArgReader.h"

ArgReader::ArgReader(int argc, char* argv[])
{
    callHelp = false;
    inputFile = "";
    outputFile = "";

    const std::string inputFlag = "-i";
    const std::string outputFlag = "-o";
    const std::string helpFlag = "-h";
    const std::string genFlag = "-g";

    const std::string help =
        "\nCommand line help:\n"
        "-i\t: tape input file (-i input.tape)\n"
        "-o\t: tape output file (-o output.tape)\n"
        "-g\t: generate tape file with n random values (-g 20 output.tape)\n"
        "-h\t: display this message\n\n";

    for (int i = 0; i < argc; i++)
    {
        // generate n random values
        if (argv[i] == genFlag && i + 2 < argc) {
            size_t size = 0;
            std::stringstream stream(argv[i + 1]);
            stream >> size;
            if (!size)
                throw std::invalid_argument("Wrong generation size.");

            generateSize = size;
            fs::path gen(argv[i + 2]);
            generateFile = gen;
            inputFile = "";
            outputFile = "";
            break;
        }

        // input file
        if (argv[i] == inputFlag && i + 1 < argc) {
            fs::path in(argv[i + 1]);
            if (!fs::exists(in))
                throw std::invalid_argument("Wrong argument or the input file doesn't exist! Use -h for help.");

            inputFile = in;
        }
        // output file
        if (argv[i] == outputFlag && i + 1 < argc) {
            fs::path out(argv[i + 1]);
            outputFile = out;
        }
        // help
        if (argv[i] == helpFlag) {
            std::cout << help;
            callHelp = true;
            break;
        }
    }

    if ((inputFile == "" || outputFile == "") && generateFile == "" && !callHelp)
        throw std::invalid_argument("Wrong input! Use -h for help.");
}