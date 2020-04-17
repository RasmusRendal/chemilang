#include "frontend.h"

void Frontend::GenerateStringStream() {
	stream.str(drv->out);
}

void Frontend::WriteFile() {
  GenerateStringStream();
	if (outputFileName.empty()) {
		outputFileName = defaultFilename;
	}
	fileStream.open(outputFileName);
	fileStream << stream.rdbuf();
	fileStream.close();
	std::cout << "file generated" << std::endl;
}
