#include "driver.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string>

enum Error {
	helpArgument = 0,
	fileError = 1,
};

class Frontend {
public:
	driver *drv;
	std::stringstream stream;
	static void Helper(Error errorCode = helpArgument);
	void GenerateStringStream();
	void WriteFile();
	std::string outputFileName = "out.crn";

private:
	std::ofstream fileStream;
};
