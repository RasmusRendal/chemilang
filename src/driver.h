#pragma once
#include "module.h"
#include "parser.hpp"
#include <map>
#include <string>
#include <vector>

// Give Flex the prototype of yylex we want ...
#define YY_DECL yy::parser::symbol_type yylex(driver &drv)
// ... and declare it for the parser's sake.
YY_DECL;

struct NoMainModuleException : public std::exception {
	const char *what() const throw() {
		return "No main module declared";
	}
};

// Conducting the whole scanning and parsing of Calc++.
//! driver class, wroom wroom.
class driver {
public:
	driver();

	// Run the parser on file F.  Return 0 on success.
	int parse_file(const std::string &filename);
	int parse_string(const std::string &s);
	int parse();
	std::string Compile();
	void FinishParsingModule();
	void FinishParsingFunction();
	std::map<std::string, Module> modules;
	Module currentModule;
	// Whether to generate parser debug traces.
	bool trace_parsing;

	// Handling the scanner.
	void scan_begin(const std::string &instream);
	void scan_end();
	// Whether to generate scanner debug traces.
	bool trace_scanning;
	// The token's location used by the scanner.
	yy::location location;
	// Removes all instances of the import regex, and parses the files
	std::string import_files(const std::string &in);
};
