#include "driver.h"
#include "frontend.h"
#include "parser.hpp"
#include <boost/algorithm/string.hpp>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <regex>
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

// TODO: This is not the best way of doing this
// If we have time, we should implement it properly in flex/
// bison or as part of a real preprocessor
std::string driver::import_files(const std::string &in) {
	std::smatch m;
	std::regex e("import ([/a-zA-Z0-9.]+);");
	std::string res = in;
	while (std::regex_search(res, m, e)) {
		const std::string &filename = m[1];
		std::ifstream f(filename);
		if (f.good()) {
			parse_file(filename);
		} else {
			parse_file(FindFileInPath(filename));
		}
		res = m.prefix().str() + m.suffix().str();
	}
	return res;
}

int driver::parse_string(const std::string &s) {
	scan_begin(import_files(s));
	return parse();
}

int driver::parse() {
	yy::parser parse(*this);
	parse.set_debug_level(
			static_cast<yy::parser::debug_level_type>(trace_parsing));
	int res = parse();
	scan_end();
	return res;
}

std::string driver::Compile() {
	if (modules.find("main") == modules.end()) {
		std::cerr << "Modules declared:" << std::endl;
		for (const auto &m : modules) {
			std::cerr << "\t" << m.first << std::endl;
		}
		throw NoMainModuleException();
	}
	std::string res = "#!/usr/bin/env -S crnsimul -e -P ";
	res += modules["main"].Compile();
	return res;
};

void driver::FinishParsingModule() {
	currentModule.Verify();
	modules.insert(std::make_pair(currentModule.name, currentModule));
	currentModule = Module();
}

void driver::FinishParsingFunction() {
	currentModule.VerifyFunction();
	modules.insert(std::make_pair(currentModule.name, currentModule));
	currentModule = Module();
}

std::string driver::FindFileInPath(const std::string &fileName) {
	char *chemPath = getenv("CHEMPATH");
	std::string searchPath = defaultPath;

	if (chemPath != nullptr) {
		searchPath = std::string(chemPath) + ":" + searchPath;
	}

	std::vector<std::string> paths;
	boost::split(paths, searchPath, [](char c) { return c == ':'; });
	for (std::string &dir : paths) {
		if (dir.back() != '/')
			dir += "/";
		std::ifstream f(dir + fileName);
		if (f.good()) {
			return dir + fileName;
		}
	}
	throw std::runtime_error("File '" + fileName + "' not found");
}
