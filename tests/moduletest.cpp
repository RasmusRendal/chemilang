#include <gtest/gtest.h>
#include "module.h"
#include "driver.h"
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
	m.species.push_back("a");
	m.species.push_back("b");
	m.species.push_back("c");

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
