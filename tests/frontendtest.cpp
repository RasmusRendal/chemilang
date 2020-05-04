#include "frontend.h"
#include "driver.h"
#include <gtest/gtest.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

class FrontendTest : public ::testing::Test {
protected:
	void SetUp() override {}

	void TearDown() override {
		// Code here will be called immediately after each test
		// (right before the destructor).
	}
};

TEST_F(FrontendTest, stringstreamTest) {
	driver drv;
	Frontend front;
	std::string in = "module main {\n"
									 "private: [x, y];\n"
									 "output: z;\n"
									 "concentrations: {\n"
									 "x := 50;\n"
									 "y := 30;\n"
									 "}\n"
									 "reactions: {\n"
									 "x + y -> x + y + z;\n"
									 "z -> 0;\n"
									 "}\n"
									 "}\n";
	std::string out = "#!/usr/bin/env -S crnsimul -e -P -C z\n"
										"x := 50;\n"
										"y := 30;\n"
										"x + y -> x + y + z;\n"
										"z -> 0;\n";
	drv.parse_string(in);
	front.drv = &drv;
	front.GenerateStringStream();
	std::stringstream expected;
	expected.str(out);
	EXPECT_EQ(front.stream.str(), expected.str());
}

TEST_F(FrontendTest, stringstreamTestNotEqual) {
	driver drv;
	Frontend front;
	std::string in = "module main {\n"
									 "private: [x, y];\n"
									 "output: z;\n"
									 "concentrations: {\n"
									 "x := 55;\n"
									 "y := 30;\n"
									 "}\n"
									 "reactions: {\n"
									 "x + y -> x + y + z;\n"
									 "z -> 0;\n"
									 "}\n"
									 "}\n";
	std::string out = "#!/usr/bin/env crnsimul\n"
										"x := 50;\n"
										"y := 30;\n"
										"x + y -> x + y + z;\n"
										"z -> 0;\n";
	drv.parse_string(in);
	front.drv = &drv;
	front.GenerateStringStream();
	std::stringstream expected;
	expected.str(out);
	EXPECT_NE(front.stream.str(), expected.str());
}
