#include "driver.h"
#include "frontend.h"
#include "parser.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

driver::driver() : trace_parsing(false), trace_scanning(false) {}

int driver::parse_file(const std::string &filename) {
	std::ifstream fileStream(filename.c_str());
	if (!fileStream.good()) {
		Frontend::Exception(fileError, filename);
		return 1;
	}
	std::stringstream buffer;
	buffer << fileStream.rdbuf();
	return parse_string(buffer.str());
}

int driver::parse_string(const std::string &s) {
	scan_begin(s);
	return parse();
}

int driver::parse() {
	yy::parser parse(*this);
	parse.set_debug_level(
			static_cast<yy::parser::debug_level_type>(trace_parsing));
	int res = parse();
	scan_end();
	if (res != 0) {
		return res;
	}
	if (modules.find("main") == modules.end()) {
		throw NoMainModuleException();
	}
	out = "#!/usr/bin/env -S crnsimul -e -P ";
	out += modules["main"].Compile();
	return res;
}

void driver::FinishParsingModule() {
	modules.insert(std::make_pair(currentModule.name, currentModule));
	currentModule = Module();
}

void driver::FinishParsingFunction() {
	currentModule.VerifyFunction();
	modules.insert(std::make_pair(currentModule.name, currentModule));
	currentModule = Module();
}
