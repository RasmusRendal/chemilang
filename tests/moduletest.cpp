#include "module.h"
#include "driver.h"
#include <gtest/gtest.h>
#include <iostream>

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
		reaction r(leftSide, rightSide, 1);
		m.reactions.push_back(r);
	}
	{
		speciesRatios leftSide;
		leftSide.insert(std::make_pair("c", 1));
		speciesRatios rightSide;
		reaction r(leftSide, rightSide, 1);
		m.reactions.push_back(r);
	}

	m.concentrations.insert(std::make_pair("a", 50));
	m.concentrations.insert(std::make_pair("b", 30));

	std::string output = "main_a := 50;\n"
											 "main_b := 30;\n"
											 "main_a + main_b -> main_a + main_b + main_c;\n"
											 "main_c -> 0;\n";

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
		reaction r(leftSide, rightSide, 1);
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
		reaction r(leftSide, rightSide, 1);
		a.reactions.push_back(r);
	}
	{
		// y -> z
		speciesRatios leftSide;
		leftSide.insert(std::make_pair("y", 1));
		speciesRatios rightSide;
		rightSide.insert(std::make_pair("z", 1));
		reaction r(leftSide, rightSide, 1);
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
		composition c(&a, inputMap, outputMap);
		main.compositions.push_back(c);
	}

	{
		// v = a(z, o);
		speciesMapping inputMap;
		inputMap.insert(std::make_pair("x", "z"));
		inputMap.insert(std::make_pair("y", "o"));
		speciesMapping outputMap;
		outputMap.insert(std::make_pair("z", "v"));
		composition c(&a, inputMap, outputMap);
		main.compositions.push_back(c);
	}

	main.ApplyCompositions();
	EXPECT_FALSE(main.reactions.empty());

	{
		const auto &reaction = main.reactions.at(0);
		const auto &leftSide = std::get<0>(reaction);
		const auto &rightSide = std::get<1>(reaction);
		EXPECT_NO_THROW(leftSide.at("z"));
		EXPECT_NO_THROW(rightSide.at("v"));
	}
	{
		const auto &reaction = main.reactions.at(1);
		const auto &leftSide = std::get<0>(reaction);
		const auto &rightSide = std::get<1>(reaction);
		EXPECT_NO_THROW(leftSide.at("o"));
		EXPECT_NO_THROW(rightSide.at("v"));
	}
	{
		const auto &reaction = main.reactions.at(2);
		const auto &leftSide = std::get<0>(reaction);
		const auto &rightSide = std::get<1>(reaction);
		EXPECT_NO_THROW(leftSide.at("x"));
		EXPECT_NO_THROW(rightSide.at("o"));
	}
	{
		const auto &reaction = main.reactions.at(3);
		const auto &leftSide = std::get<0>(reaction);
		const auto &rightSide = std::get<1>(reaction);
		EXPECT_NO_THROW(leftSide.at("y"));
		EXPECT_NO_THROW(rightSide.at("o"));
	}
}
