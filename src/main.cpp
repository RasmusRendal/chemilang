#include <iostream>
#include <string>
#include "driver.h"

int main(int argc, char *argv[]) {
	std::cout << "Hello world" << std::endl;
	std::string filename = argv[1];
	std::cout << "Compiling " + filename << std::endl;
	driver drv;
	drv.parse_file(filename);
	if (!drv.parse_file(filename)) {
		std::cout << "Something went wrong" << std::endl;
	}

	return drv.result;
}
