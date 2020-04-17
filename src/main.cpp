#include "driver.h"
#include "frontend.h"
#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
	Frontend frontend;
	std::string filename = argv[1];
	driver drv;
	int res = drv.parse_file(filename);
	if (res != 0) {
		std::cout << "Compilation error" << std::endl;
	} else {
		for (int i = 1; i < argc; ++i) {
			if (argv[i] == std::string("-o"))
				frontend.outputFileName = std::string(argv[++i]);
		}
		frontend.drv = &drv;
		frontend.WriteFile();
	}
	return res;
}
