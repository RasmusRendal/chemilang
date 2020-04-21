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

	try {
		if (argv[argc - 1] == std::string("-h")) {
			Frontend::Helper(helpArgument);
			return 0;
		}
		for (int i = 1; i < argc; ++i) {
			if (file_included(argv[i])) {
				filename = argv[i];
			} else if (argv[i] == std::string("-o")) {
				frontend.outputFileName = std::string(argv[++i]);
			} else {
				throw std::invalid_argument(argv[i]);
			}
		}

		if (drv.parse_file(filename) == 0) {
			frontend.drv = &drv;
			frontend.WriteFile();
		} else {
			std::cout << "Compilation error" << std::endl;
			Frontend::Helper();
		}
		return 0;
	} catch (std::invalid_argument &ex) {
		std::cout << "Invalid command-line argument used " << ex.what() << '\n';
		std::cout << "Please see -h for more info" << std::endl;
	}
}
