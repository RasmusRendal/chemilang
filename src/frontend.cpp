#include "frontend.h"

void Frontend::GenerateStringStream() {
	stream.str(drv->out);
}

void Frontend::WriteFile() {
	GenerateStringStream();
	fileStream.open(outputFileName);
	fileStream << stream.rdbuf();
	fileStream.close();
	std::cout << "Output written to " << outputFileName << std::endl;
}
