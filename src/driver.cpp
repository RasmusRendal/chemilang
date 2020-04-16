#include "driver.h"
#include "parser.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

driver::driver() : trace_parsing(false), trace_scanning(false) {}

int driver::parse_file(const std::string &filename) {
	std::ifstream fileStream(filename.c_str());
	if (!fileStream.good()) {
		std::cout << "File not found" << std::endl;
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
	if (modules.find("main") == modules.end()) {
		throw std::runtime_error("No main module;");
	}
	out = "#!/usr/bin/env crnsimul\n";
	out += modules["main"].Compile();
	return res;
}

void driver::FinishParsingModule() {
	modules.insert(std::make_pair(currentModule.name, currentModule));
	currentModule = Module();
}
