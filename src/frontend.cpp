#include "frontend.h"

void Frontend::GenerateStringStream() {
	stream.str(drv->out);
}

bool Frontend::IsValidPath(const std::string &filePath) {
	if (filePath[0] == '-') {
		Exception(argError, filePath);
		return false;
	}

	std::ifstream fileStream(filePath.c_str());
	if (fileStream.good()) {
		return true;
	}

	Frontend::Exception(fileError, filePath);

	return false;
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
		if (input[0] != '-') {
			std::cout << "Error: No file for parsing or file not found: " << input
								<< std::endl;
			break;
		} else {
			break;
		}
	case argError:
		if (input[0] == '-') {
			std::cout << "Invalid command-line argument used: " << input << "\n";
			std::cout << "Please see -h for more info" << std::endl;
			break;
		} else {
			break;
		}
	case outFileError:
		std::cout << "Error: No output file specified for option: " << input
							<< std::endl;
		break;
	}
}

void Frontend::PrintHelper() {
	std::string helperstring = "Usage:  chemilang [OPTIONS] filename\n"
														 "Options:\n"
														 "    -o  Output filename Usage: -o [FILNAME]\n"
														 "    -h  Display help information";
	std::cout << helperstring << std::endl;
};
