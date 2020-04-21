#include "frontend.h"

void Frontend::GenerateStringStream() {
	stream.str(drv->out);
}

void Frontend::WriteFile() {
	GenerateStringStream();
	fileStream.open(outputFileName);
	fileStream << stream.rdbuf();
	fileStream.close();
	std::cout << "Output written to " << outputFileName << std::endl;
}

void Frontend::Helper(Error errorCode) {
	if (errorCode == fileError) {
		std::cout << "Error: No file for parsing" << std::endl;
	}
	std::string helperstring = "Usage:  chemilang filename [OPTIONS]\n"
														 "Options:\n"
														 "    -o  Output filename\n"
														 "    -h  Display help information";
	std::cout << helperstring << std::endl;
};
