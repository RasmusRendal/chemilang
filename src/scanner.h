#pragma once

#include <FlexLexer.h>

class driver;

class Scanner : public yyFlexLexer {
public:
	Scanner(driver &mDriver) : mDriver(mDriver) {}
	virtual ~Scanner() {}
	virtual EzAquarii::Parser::symbol_type get_next_token();

private:
	driver mDriver;
};
}
