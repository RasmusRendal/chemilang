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

TEST_F(BasicTest, CompWrongInputSize) {
	std::string in = "module Addition {\n"
									 "input: x;\n"
									 "output: z;\n"
									 "reactions: {\n"
									 "x -> z;\n"
									 "}\n"
									 "}\n"
									 "module main {\n"
									 "private: [a, b];\n"
									 "output: c;\n"
									 "concentrations: {\n"
									 "a := 50;\n"
									 "b := 50;\n"
									 "}\n"
									 "compositions: {\n"
									 "c = Addition(a, b);\n"
									 "}\n"
									 "}\n";

	driver drv;
	ASSERT_THROW(drv.parse_string(in), CompositionException);
}

TEST_F(BasicTest, CompWrongOutputSize) {
	std::string in = "module Addition {\n"
									 "input: x;\n"
									 "output: z;\n"
									 "reactions: {\n"
									 "x -> z;\n"
									 "}\n"
									 "}\n"
									 "module main {\n"
									 "private: a;\n"
									 "output: [b, c];\n"
									 "concentrations: {\n"
									 "a := 50;\n"
									 "}\n"
									 "compositions: {\n"
									 "b, c = Addition(a);\n"
									 "}\n"
									 "}\n";

	driver drv;
	ASSERT_THROW(drv.parse_string(in), CompositionException);
}

TEST_F(BasicTest, NestedComposition) {
	std::string input = "module thing {\n"
		"input: c;\n"
		"output: j;\n"
		"private: chem;\n"
		"concentrations: {\n"
			"chem := 420;\n"
			"}\n"
		"}\n"
		"module some {\n"
			"input: x;\n"
			"private: aa;\n"
			"output: b;\n"
			"concentrations: {\n"
				"aa := 20;\n"
			"}\n"
			"compositions: {\n"
				"b = thing(aa);\n"
		"}\n"
		"}\n"
		"module main {\n"
			"input: b;\n"
			"output: z;\n"
			"private: a;\n"
			"concentrations: {\n"
				"a := 1337;\n"
			"}\n"
			"compositions: {\n"
				"z = some(a);\n"
			"}\n"
		"}\n";
		driver drv;
		drv.parse_string(input);
		std::string expected = "#!/usr/bin/env crnsimul\n"
			"main_a := 1337;\n"
			"main_some_0_aa := 20;\n"
			"main_some_0_thing_0_chem := 420;\n";
		EXPECT_EQ(drv.out, expected);
}

TEST_F(BasicTest, privateSpecMapOneSubMod) {
	std::string in = "module Addition {\n"
									 "input: x;\n"
									 "private: y;\n"
									 "output: z;\n"
									 "concentrations: {\n"
									 "y := 20;\n"
									 "}\n"
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
									 "d = Addition(a);\n"
									 "}\n"
									 "}\n";

	std::string out = "#!/usr/bin/env crnsimul\n"
										"main_Addition_0_y := 20;\n"
										"main_a := 50;\n"
										"main_b := 30;\n"
										"main_c := 30;\n"
										"main_a -> main_d;\n"
										"main_Addition_0_y -> main_d;\n";
	driver drv;
	ASSERT_EQ(drv.parse_string(in), 0);
	EXPECT_EQ(drv.out, out);
}

TEST_F(BasicTest, privateSpecMapTwoSubMod) {
	std::string in = "module AdditionTwo {\n"
									 "input: x;\n"
									 "private: y;\n"
									 "output: z;\n"
									 "concentrations: {\n"
									 "y := 20;\n"
									 "}\n"
									 "reactions: {\n"
									 "x -> z;\n"
									 "y -> z;\n"
									 "}\n"
									 "}\n"
									 "module Addition {\n"
									 "input: x;\n"
									 "private: y;\n"
									 "output: z;\n"
									 "concentrations: {\n"
									 "y := 20;\n"
									 "}\n"
									 "reactions: {\n"
									 "x -> z;\n"
									 "y -> z;\n"
									 "}\n"
									 "compositions: {\n"
									 "z = AdditionTwo(y);\n"
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
									 "d = Addition(a);\n"
									 "}\n"
									 "}\n";

	std::string out = "#!/usr/bin/env crnsimul\n"
										"main_Addition_0_AdditionTwo_0_y := 20;\n"
										"main_Addition_0_y := 20;\n"
										"main_a := 50;\n"
										"main_b := 30;\n"
										"main_c := 30;\n"
										"main_a -> main_d;\n"
										"main_Addition_0_y -> main_d;\n"
										"main_Addition_0_y -> main_d;\n"
										"main_Addition_0_AdditionTwo_0_y -> main_d;\n";
	driver drv;
	ASSERT_EQ(drv.parse_string(in), 0);
	EXPECT_EQ(drv.out, out);
}

TEST_F(BasicTest, privateSpecSubModReacConc) {
	std::string in = "module Addition {\n"
									 "input: x;\n"
									 "private: y;\n"
									 "output: z;\n"
									 "concentrations: {\n"
									 "y := 20;\n"
									 "}\n"
									 "reactions: {\n"
									 "5x -> z;\n"
									 "10y -> z;\n"
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
									 "d = Addition(a);\n"
									 "}\n"
									 "}\n";

	std::string out = "#!/usr/bin/env crnsimul\n"
										"main_Addition_0_y := 20;\n"
										"main_a := 50;\n"
										"main_b := 30;\n"
										"main_c := 30;\n"
										"5main_a -> main_d;\n"
										"10main_Addition_0_y -> main_d;\n";
	driver drv;
	ASSERT_EQ(drv.parse_string(in), 0);
	EXPECT_EQ(drv.out, out);
}

TEST_F(BasicTest, outputSpecInSubModConc) {
	std::string in = "module Addition {\n"
									 "input: x;\n"
									 "private: y;\n"
									 "output: z;\n"
									 "concentrations: {\n"
									 "y := 20;\n"
									 "z := 20;\n"
									 "}\n"
									 "reactions: {\n"
									 "5x -> z;\n"
									 "10y -> z;\n"
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
									 "d = Addition(a);\n"
									 "}\n"
									 "}\n";

	std::string out = "#!/usr/bin/env crnsimul\n"
										"main_Addition_0_y := 20;\n"
										"main_Addition_0_z := 20;\n"
										"main_a := 50;\n"
										"main_b := 30;\n"
										"main_c := 30;\n"
										"5main_a -> main_d;\n"
										"10main_Addition_0_y -> main_d;\n";
	driver drv;
	ASSERT_EQ(drv.parse_string(in), 0);
	EXPECT_EQ(drv.out, out);
}