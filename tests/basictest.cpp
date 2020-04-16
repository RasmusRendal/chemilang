#include "driver.h"
#include <gtest/gtest.h>
#include <stdexcept>

class BasicTest : public ::testing::Test {
protected:
	void SetUp() override {}

	void TearDown() override {
		// Code here will be called immediately after each test
		// (right before the destructor).
	}
};

TEST_F(BasicTest, CompileExample) {
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
	std::string out = "#!/usr/bin/env crnsimul\n"
										"main_x := 50;\n"
										"main_y := 30;\n"
										"main_x + main_y -> main_x + main_y + main_z;\n"
										"main_z -> 0;\n";
	driver drv;
	ASSERT_EQ(drv.parse_string(in), 0);
	EXPECT_EQ(drv.out, out);
	const int x = 4;
	const int y = 5;
	EXPECT_EQ(9, x + y);
}

TEST_F(BasicTest, NoMainModule) {
	std::string in = "module notmain {\n"
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

	driver drv;
	ASSERT_THROW(drv.parse_string(in), NoMainModuleException);
}
