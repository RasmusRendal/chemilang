#include "module.h"
#include "driver.h"
#include <gtest/gtest.h>
#include <iostream>
#include <string>

class ModuleTest : public ::testing::Test {
protected:
	void SetUp() override {}

	void TearDown() override {
		// Code here will be called immediately after each test
		// (right before the destructor).
	}
};

TEST_F(ModuleTest, twoValues) {
	Module m;
	m.name = "main";
	m.privateSpecies.push_back("a");
	m.privateSpecies.push_back("b");
	m.privateSpecies.push_back("c");

	{
		speciesRatios leftSide;
		leftSide.insert(std::make_pair("a", 1));
		leftSide.insert(std::make_pair("b", 1));
		speciesRatios rightSide;
		rightSide.insert(std::make_pair("a", 1));
		rightSide.insert(std::make_pair("b", 1));
		rightSide.insert(std::make_pair("c", 1));
		reaction r = {leftSide, rightSide, 1};
		m.reactions.push_back(r);
	}
	{
		speciesRatios leftSide;
		leftSide.insert(std::make_pair("c", 1));
		speciesRatios rightSide;
		reaction r = {leftSide, rightSide, 1};
		m.reactions.push_back(r);
	}

	m.concentrations.insert(std::make_pair("a", 50));
	m.concentrations.insert(std::make_pair("b", 30));

	std::string output = "\na := 50;\n"
											 "b := 30;\n"
											 "a + b -> a + b + c;\n"
											 "c -> 0;\n";

	EXPECT_EQ(m.Compile(), output);
}

TEST_F(ModuleTest, SpecieNotDeclared) {
	Module m;
	m.name = "main";
	m.concentrations.insert(std::make_pair("a", 50));
	EXPECT_THROW(m.Verify(), SpecieNotDeclaredException);
}

TEST_F(ModuleTest, SpecieNotDeclaredConcentrations) {
	Module m;
	m.name = "main";
	{
		speciesRatios leftSide;
		leftSide.insert(std::make_pair("c", 1));
		speciesRatios rightSide;
		reaction r = {leftSide, rightSide, 1};
		m.reactions.push_back(r);
	}
	EXPECT_THROW(m.Verify(), SpecieNotDeclaredException);
}

TEST_F(ModuleTest, ApplyCompositionsTest) {
	Module a;
	a.inputSpecies.push_back("x");
	a.inputSpecies.push_back("y");
	a.outputSpecies.push_back("z");
	{
		// x -> z
		speciesRatios leftSide;
		leftSide.insert(std::make_pair("x", 1));
		speciesRatios rightSide;
		rightSide.insert(std::make_pair("z", 1));
		reaction r = {leftSide, rightSide, 1};
		a.reactions.push_back(r);
	}
	{
		// y -> z
		speciesRatios leftSide;
		leftSide.insert(std::make_pair("y", 1));
		speciesRatios rightSide;
		rightSide.insert(std::make_pair("z", 1));
		reaction r = {leftSide, rightSide, 1};
		a.reactions.push_back(r);
	}

	Module main;
	main.name = "main";
	main.inputSpecies.push_back("x");
	main.inputSpecies.push_back("y");
	main.inputSpecies.push_back("z");

	main.outputSpecies.push_back("v");
	main.privateSpecies.push_back("o");

	{
		// o = a(x, y);
		speciesMapping inputMap;
		inputMap.insert(std::make_pair("x", "x"));
		inputMap.insert(std::make_pair("y", "y"));
		speciesMapping outputMap;
		outputMap.insert(std::make_pair("z", "o"));
		auto c = new ModuleComposition(&a, inputMap, outputMap);
		main.compositions.push_back(c);
	}

	{
		// v = a(z, o);
		speciesMapping inputMap;
		inputMap.insert(std::make_pair("x", "z"));
		inputMap.insert(std::make_pair("y", "o"));
		speciesMapping outputMap;
		outputMap.insert(std::make_pair("z", "v"));
		auto c = new ModuleComposition(&a, inputMap, outputMap);
		main.compositions.push_back(c);
	}

	main.ApplyCompositions();
	EXPECT_FALSE(main.reactions.empty());

	{
		const auto &reaction = main.reactions.at(0);
		const auto &leftSide = reaction.reactants;
		const auto &rightSide = reaction.products;
		EXPECT_NO_THROW(leftSide.at("z"));
		EXPECT_NO_THROW(rightSide.at("v"));
	}
	{
		const auto &reaction = main.reactions.at(1);
		const auto &leftSide = reaction.reactants;
		const auto &rightSide = reaction.products;
		EXPECT_NO_THROW(leftSide.at("o"));
		EXPECT_NO_THROW(rightSide.at("v"));
	}
	{
		const auto &reaction = main.reactions.at(2);
		const auto &leftSide = reaction.reactants;
		const auto &rightSide = reaction.products;
		EXPECT_NO_THROW(leftSide.at("x"));
		EXPECT_NO_THROW(rightSide.at("o"));
	}
	{
		const auto &reaction = main.reactions.at(3);
		const auto &leftSide = reaction.reactants;
		const auto &rightSide = reaction.products;
		EXPECT_NO_THROW(leftSide.at("y"));
		EXPECT_NO_THROW(rightSide.at("o"));
	}
}

TEST_F(ModuleTest, ReactionRate) {
	Module m;
	m.name = "main";
	m.privateSpecies.push_back("a");
	m.privateSpecies.push_back("b");
	m.privateSpecies.push_back("c");

	{
		speciesRatios leftSide;
		leftSide.insert(std::make_pair("a", 1));
		leftSide.insert(std::make_pair("b", 1));
		speciesRatios rightSide;
		rightSide.insert(std::make_pair("a", 1));
		rightSide.insert(std::make_pair("b", 1));
		rightSide.insert(std::make_pair("c", 1));
		reaction r = {leftSide, rightSide, 2};
		m.reactions.push_back(r);
	}
	{
		speciesRatios leftSide;
		leftSide.insert(std::make_pair("c", 1));
		speciesRatios rightSide;
		reaction r = {leftSide, rightSide, 1};
		m.reactions.push_back(r);
	}

	m.concentrations.insert(std::make_pair("a", 50));
	m.concentrations.insert(std::make_pair("b", 30));

	std::string output = "\na := 50;\n"
											 "b := 30;\n"
											 "a + b ->(2) a + b + c;\n"
											 "c -> 0;\n";

	EXPECT_EQ(m.Compile(), output);
}
TEST_F(ModuleTest, ReactionRateTrailingZeros) {
	Module m;
	m.name = "main";
	m.privateSpecies.emplace_back("a");
	m.privateSpecies.emplace_back("b");
	m.privateSpecies.emplace_back("c");

	{
		speciesRatios leftSide;
		leftSide.insert(std::make_pair("a", 1));
		leftSide.insert(std::make_pair("b", 1));
		speciesRatios rightSide;
		rightSide.insert(std::make_pair("a", 1));
		rightSide.insert(std::make_pair("b", 1));
		rightSide.insert(std::make_pair("c", 1));
		reaction r = {leftSide, rightSide, 2.300000};
		m.reactions.push_back(r);
	}
	{
		speciesRatios leftSide;
		leftSide.insert(std::make_pair("c", 1));
		speciesRatios rightSide;
		reaction r = {leftSide, rightSide, 1};
		m.reactions.push_back(r);
	}

	m.concentrations.insert(std::make_pair("a", 50));
	m.concentrations.insert(std::make_pair("b", 30));

	std::string output = "\na := 50;\n"
											 "b := 30;\n"
											 "a + b ->(2.3) a + b + c;\n"
											 "c -> 0;\n";

	EXPECT_EQ(m.Compile(), output);
}

TEST_F(ModuleTest, InputConcException) {
	Module a;
	a.inputSpecies.push_back("x");
	a.outputSpecies.push_back("z");

	a.concentrations.insert(std::make_pair("x", 50));

	{
		// x + y -> y
		speciesRatios leftSide;
		leftSide.insert(std::make_pair("x", 1));
		speciesRatios rightSide;
		rightSide.insert(std::make_pair("z", 1));
		reaction r = {leftSide, rightSide, 1};
		a.reactions.push_back(r);
	}

	Module main;
	main.name = "main";
	main.inputSpecies.push_back("x");

	main.outputSpecies.push_back("z");

	{
		// z = a(x);
		speciesMapping inputMap;
		inputMap.insert(std::make_pair("x", "x"));
		speciesMapping outputMap;
		outputMap.insert(std::make_pair("z", "z"));
		auto c = new ModuleComposition(&a, inputMap, outputMap);
		main.compositions.push_back(c);
	}

	EXPECT_THROW(main.ApplyCompositions(), InputSpecieConcException);
}

TEST_F(ModuleTest, MultSpeciesInReact) {
	Module m;
	m.name = "main";
	m.privateSpecies.push_back("a");
	m.privateSpecies.push_back("c");

	{
		speciesRatios leftSide;
		leftSide.insert(std::make_pair("a", 2));
		speciesRatios rightSide;
		rightSide.insert(std::make_pair("c", 1));
		reaction r = {leftSide, rightSide, 1};
		m.reactions.push_back(r);
	}
	{
		speciesRatios leftSide;
		leftSide.insert(std::make_pair("c", 1));
		speciesRatios rightSide;
		reaction r = {leftSide, rightSide, 1};
		m.reactions.push_back(r);
	}

	m.concentrations.insert(std::make_pair("a", 50));

	std::string output = "\na := 50;\n"
											 "2a -> c;\n"
											 "c -> 0;\n";

	EXPECT_EQ(m.Compile(), output);
}

/*
 * Commented out, added as issue #72
TEST_F(ModuleTest, FunctionTest) {
	std::string input = "function funcm { \n"
											"input: c; \n"
											"private: b; \n"
											"output: a; \n"
											"reactions: {\n"
											"c + a -> b + a + c; \n } \n"
											"}"
											"module main { \n"
											"private: a; \n"
											"concentrations: { \n"
											"a := 5; \n } \n"
											"compositions: {\n"
											" a = funcm(a); \n "
											"} \n}";
	driver drv;
	drv.parse_string(input);
	std::string expected = "#!/usr/bin/env -S crnsimul -e -P \n"
												 "a := 5;\n"
												 "a + funcm_0_b -> a;\n";
	EXPECT_EQ(drv.Compile(), expected);
} */

TEST_F(ModuleTest, FunctionTestThrow) {
	std::string input = "function funcm { \n"
											"input: c; \n"
											"private: b; \n"
											"output: a; \n"
											"reactions: {\n"
											"c + a -> b + a; \n } \n"
											"}"
											"module main { \n"
											"private: a; \n"
											"concentrations: { \n"
											"a := 5; \n } \n"
											"compositions: {\n"
											" a = funcm(a); \n "
											"} \n}";
	driver drv;
	EXPECT_THROW(drv.parse_string(input), FunctionIncorrectReactionsException);
}

TEST_F(ModuleTest, FunctionTestRatioDifferent) {
	std::string input = "function funcm { \n"
											"input: c; \n"
											"private: b; \n"
											"output: a; \n"
											"reactions: {\n"
											"c + a -> b + a + 3c; \n } \n"
											"}"
											"module main { \n"
											"private: a; \n"
											"concentrations: { \n"
											"a := 5; \n } \n"
											"compositions: {\n"
											" a = funcm(a); \n "
											"} \n}";
	driver drv;
	EXPECT_THROW(drv.parse_string(input), FunctionIncorrectReactionsException);
}

/*
 * Also commented out as per issue #72
TEST_F(ModuleTest, FunctionTestInputSpeciesVariantPresence) {
	std::string input = "function funcm { \n"
											"input: c; \n"
											"private: b; \n"
											"output: a; \n"
											"reactions: {\n"
											"c + a -> b + a + c; \n"
											"a -> 0; \n } \n"
											"}"
											"module main { \n"
											"private: a; \n"
											"concentrations: { \n"
											"a := 5; \n } \n"
											"compositions: {\n"
											" a = funcm(a); \n "
											"} \n}";
	std::string expected = "#!/usr/bin/env -S crnsimul -e -P \n"
												 "a := 5;\n"
												 "a + funcm_0_b -> a;\n"
												 "a -> 0;\n";
	driver drv;
	drv.parse_string(input);
	EXPECT_EQ(drv.Compile(), expected);
}
*/
