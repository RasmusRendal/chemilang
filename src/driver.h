#pragma once
#include "module.h"
#include "function.h"
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
	void FinishParsingModule();
	void FinishParsingFunction();
	// The name of the file being parsed.
	std::string file;
	std::string out = "";
	std::map<std::string, Module> modules;
	std::map<std::string, Function> functions;
	Module currentModule;
	Function currentFunction;
	// Whether to generate parser debug traces.
	bool trace_parsing;

	// Handling the scanner.
	void scan_begin(const std::string &instream);
	void scan_end();
	// Whether to generate scanner debug traces.
	bool trace_scanning;
	// The token's location used by the scanner.
	yy::location location;
};
