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

TEST_F(BasicTest, TwoModules) {
	std::string in = "module useless {\n"
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
									 "}\n"
									 "module main {\n"
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
}

TEST_F(BasicTest, CompositionTest) {
	std::string in = "module Addition {\n"
									 "input: [x, y];\n"
									 "output: z;\n"
									 "reactions: {\n"
									 "x -> z;\n"
									 "y -> z;\n"
									 "}\n"
									 "}\n"
									 "module main {\n"
									 "private: [a, b, c, d];\n"
									 "output: e;\n"
									 "concentrations: {\n"
									 "a := 50;\n"
									 "b := 30;\n"
									 "c := 30;\n"
									 "}\n"
									 "compositions: {\n"
									 "d = Addition(a, b);\n"
									 "e = Addition(d, c);\n"
									 "}\n"
									 "}\n";

	std::string out = "#!/usr/bin/env crnsimul\n"
										"main_a := 50;\n"
										"main_b := 30;\n"
										"main_c := 30;\n"
										"main_d -> main_e;\n"
										"main_c -> main_e;\n"
										"main_a -> main_d;\n"
										"main_b -> main_d;\n";
	driver drv;
	ASSERT_EQ(drv.parse_string(in), 0);
	EXPECT_EQ(drv.out, out);
}

TEST_F(BasicTest, ReactionRateTest) {
	std::string in = "module main {\n"
									 "private: [x, y];\n"
									 "output: z;\n"
									 "concentrations: {\n"
									 "x := 50;\n"
									 "y := 30;\n"
									 "}\n"
									 "reactions: {\n"
									 "x + y ->(2) x + y + z;\n"
									 "z -> 0;\n"
									 "}\n"
									 "}\n";
	std::string out = "#!/usr/bin/env crnsimul\n"
										"main_x := 50;\n"
										"main_y := 30;\n"
										"main_x + main_y ->(2) main_x + main_y + main_z;\n"
										"main_z -> 0;\n";
	driver drv;
	ASSERT_EQ(drv.parse_string(in), 0);
	EXPECT_EQ(drv.out, out);
}

TEST_F(BasicTest, InputConcException) {
	std::string in = "module Addition {\n"
									 "input: x;\n"
									 "output: z;\n"
									 "concentrations: {\n"
									 "x := 5;\n"
									 "}\n"
									 "reactions: {\n"
									 "x -> z;\n"
									 "}\n"
									 "}\n"
									 "module main {\n"
									 "private: a;\n"
									 "output: b;\n"
									 "concentrations: {\n"
									 "a := 50;\n"
									 "}\n"
									 "compositions: {\n"
									 "b = Addition(a);\n"
									 "}\n"
									 "}\n";

	driver drv;
	ASSERT_THROW(drv.parse_string(in), InputSpecieConcException);
}

TEST_F(BasicTest, MultSpeciesInReact) {
	std::string in = "module main {\n"
									 "private: x;\n"
									 "output: y;\n"
									 "concentrations: {\n"
									 "x := 50;\n"
									 "}\n"
									 "reactions: {\n"
									 "2x -> y;\n"
									 "}\n"
									 "}\n";
	std::string out = "#!/usr/bin/env crnsimul\n"
										"main_x := 50;\n"
										"2main_x -> main_y;\n";
	driver drv;
	ASSERT_EQ(drv.parse_string(in), 0);
	EXPECT_EQ(drv.out, out);
}

TEST_F(BasicTest, MultSpeciesInReact2) {
	std::string in = "module main {\n"
									 "private: x;\n"
									 "output: y;\n"
									 "concentrations: {\n"
									 "x := 50;\n"
									 "}\n"
									 "reactions: {\n"
									 "x + x + x -> y;\n"
									 "}\n"
									 "}\n";
	std::string out = "#!/usr/bin/env crnsimul\n"
										"main_x := 50;\n"
										"3main_x -> main_y;\n";
	driver drv;
	ASSERT_EQ(drv.parse_string(in), 0);
	EXPECT_EQ(drv.out, out);
}