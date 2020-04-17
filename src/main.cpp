#include "driver.h"
#include "frontend.h"
#include <iostream>
#include <string>

bool file_included(const std::string &filename) {
	return std::ifstream(filename).good();
}

int main(int argc, char *argv[]) {
	Frontend frontend;
	std::string filename;
	driver drv;

	for(int i = 1; i < argc; ++i) {
		if (file_included(argv[i])) {
			filename = argv[i];
		} else if (argv[i] == std::string("-o")) {
			frontend.outputFileName = std::string(argv[++i]);
		}
	}

	if (drv.parse_file(filename) == 0) {
		frontend.drv = &drv;
		frontend.WriteFile();
	} else {
		std::cout << "Compilation error" << std::endl;
	}
	
	return 0;
}
