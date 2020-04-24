#include "frontend.h"
#include <ostream>
#include <sys/stat.h>

void Frontend::GenerateStringStream() {
	stream.str(drv->out);
}

void Frontend::WriteFile() {
	GenerateStringStream();
	fileStream.open(outputFileName);
	fileStream << stream.rdbuf();
	chmod(outputFileName.c_str(), S_IRWXU);
	fileStream.close();
	std::cout << "Output written to " << outputFileName << std::endl;
}

void Frontend::Exception(Error errorCode, const std::string &input) {
	switch (errorCode) {
	case helpArgument:
		Frontend::PrintHelper();
		break;
	case fileError:
		std::cout << "Error: No file for parsing or file not found: " << input
							<< std::endl;
		break;
	case argError:
		std::cout << "Invalid command-line argument used: " << input << "\n";
		std::cout << "Please see -h for more info" << std::endl;
		break;
	case outFileError:
		std::cout << "Error: No output file specified" << std::endl;
		break;
	}
}

void Frontend::PrintHelper() {
	std::string helperstring = "Usage:  chemilang filename [OPTIONS]\n"
														 "Options:\n"
														 "    -o  Output filename\n"
														 "    -h  Display help information";
	std::cout << helperstring << std::endl;
};
