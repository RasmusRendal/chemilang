#include "driver.h"
#include "frontend.h"
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
		return 0;
	}

	for (int i = 1; i < argc; ++i) {
		if (file_included(argv[i])) {
			filename = argv[i];
		} else if (argv[i] == std::string("-o") && argc == 4) {
			frontend.outputFileName = std::string(argv[++i]);
		} else if (argv[i] == std::string("-o") && argc == 3) {
			Frontend::Exception(outFileError, argv[i]);
			return 1;
		} else {
			Frontend::Exception(argError, argv[i]);
			return 1;
		}
	}

	if (drv.parse_file(filename) == 0) {
		frontend.drv = &drv;
		frontend.WriteFile();
	} else {
		std::cout << "Compilation error" << std::endl;
		return 1;
	}
	return 0;
}
