#include "driver.h"
#include "frontend.h"
#include "sysexits.h"
#include <cstdio>
#include <iostream>
#include <string>

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
		if (argv[i] == std::string("-o") && argc == 4 || argc == 3) {
			frontend.outputFileName = std::string(argv[++i]);
		} else if (argv[i] == std::string("-o") && argc == 2) {
			Frontend::Exception(outFileError, argv[i]);
			return EX_USAGE;
		}

		if (Frontend::IsValidPath(argv[i]) && argc == 4 || argc == 2) {
			filename = argv[i];
		}

		if (argc == 3) {
			Frontend::Exception(argError, argv[i]);
			return EX_USAGE;
		}
		std::cout << argv[i] << argc << i << std::endl;
	}

	int parseRes = drv.parse_file(filename);
	if (parseRes == 0) {
		frontend.drv = &drv;
		frontend.WriteFile();
	} else {
		return EX_DATAERR;
	}
	return EX_OK;
}
