#include "driver.h"
#include "frontend.h"
#include "sysexits.h"
#include <cstdio>
#include <iostream>
#include <string>

bool file_included(const std::string &filename) {
	return std::ifstream(filename).good();
}

int main(int argc, char *argv[]) {
	Frontend frontend;
	std::string filename;
	driver drv;
	if (argv[argc - 1] == std::string("-h") ||
			argv[argc - 1] == std::string("-help")) {
		Frontend::PrintHelper();
		return EX_OK;
	}

	for (int i = 1; i < argc; ++i) {
		if (file_included(argv[i])) {
			filename = argv[i];
		} else if (argv[i] == std::string("-o") && argc == 4) {
			frontend.outputFileName = std::string(argv[++i]);
		} else if (argv[i] == std::string("-o") && argc <= 3) {
			Frontend::Exception(outFileError, argv[i]);
			return EX_DATAERR;
		} else {
			Frontend::Exception(argError, argv[i]);
			return EX_DATAERR;
		}
	}

	int parseRes = drv.parse_file(filename);
	if (parseRes == 0) {
		frontend.drv = &drv;
		frontend.WriteFile();
	} else {
		return parseRes;
	}
	return EX_OK;
}
