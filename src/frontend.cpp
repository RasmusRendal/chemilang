#include "frontend.h"

void Frontend::GenerateStringStream() {
	stream.str(drv->out);
}

bool Frontend::ValidateFileStream(const std::string &filename) {
	std::ifstream fileStream(filename.c_str());
	if (!fileStream.good()) {
		Frontend::Exception(fileError, filename);
		return false;
	}
	return true;
};

void Frontend::WriteFile() {
	GenerateStringStream();
	fileStream.open(outputFileName);
	fileStream << stream.rdbuf();
	fileStream.close();
	std::cout << "Output written to " << outputFileName << std::endl;
}

void Frontend::Exception(Error errorCode, const std::string &input) {
	switch (errorCode) {
	case helpArgument:
		Frontend::PrintHelper();
		break;
	case fileError:
		if (input.find('-') == std::string::npos) {
			std::cout << "Error: No file for parsing or file not found: " << input
								<< std::endl;
			break;
		} else {
			break;
		}
	case argError:
		if (input.find('-') != std::string::npos) {
			std::cout << "Invalid command-line argument used: " << input << "\n";
			std::cout << "Please see -h for more info" << std::endl;
			break;
		} else {
			break;
		}
	case outFileError:
		std::cout << "Error: No output file specified" << std::endl;
		break;
	}
}

void Frontend::PrintHelper() {
	std::string helperstring = "Usage:  chemilang [OPTIONS]\n"
														 "Options:\n"
														 "    -o  Output filename\n"
														 "    -h  Display help information";
	std::cout << helperstring << std::endl;
};
