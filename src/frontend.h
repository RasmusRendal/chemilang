#include "driver.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string>

enum Error {
	helpArgument = 0,
	fileError = 1,
	argError = 2,
};

class Frontend {
public:
	driver *drv;
	std::stringstream stream;
	static void PrintHelper(); 
        static void Exception(Error errorCode, const std::string &input);
	void GenerateStringStream();
	void WriteFile();
	std::string outputFileName = "out.crn";

private:
	std::ofstream fileStream;
};
