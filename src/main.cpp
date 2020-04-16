#include "driver.h"
#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
	std::string filename = argv[1];
	driver drv;
	int res = drv.parse_file(filename);
	if (res != 0) {
		std::cout << "Compilation error" << std::endl;
	} else {
		std::cout << drv.out;
	}

	return res;
}
