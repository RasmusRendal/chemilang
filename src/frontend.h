#include "driver.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <string>

class Frontend {
public:
	driver *drv;
	std::stringstream stream;
	void GenerateStringStream();
	void WriteFile();
	std::string outputFileName;
private:
	std::string defaultFilename = "out.crn";
	std::ofstream fileStream;
};
