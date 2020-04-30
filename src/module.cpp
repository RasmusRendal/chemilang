#include "module.h"
#include "composition.h"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace precision {

std::string to_string(double d) {

	std::ostringstream stm;
	stm << std::setprecision(std::numeric_limits<double>::digits10) << d;
	return stm.str();
}
} // namespace precision

std::string Module::Compile() {
	Verify();
	ApplyCompositions();

	std::string output;

	if (outputSpecies.size() > 0) {
		output += "-C ";
		for (const auto &specie : outputSpecies) {
			if (name != "main") {
				output += name + "_" + specie + ",";
			} else {
				output += specie + ",";
			}
		}
		output.pop_back();
	}
	output += "\n";

	for (const auto &c : concentrations) {
		if (name != "main") {
			output +=
					name + "_" + c.first + " := " + std::to_string(c.second) + ";\n";
		} else {
			output += c.first + " := " + std::to_string(c.second) + ";\n";
		}
	}
	for (const auto &reaction : reactions) {
		if (!reaction.reactants.empty()) {
			for (const auto &specie : reaction.reactants) {
				output += MapName(name, specie.first, specie.second);
			}
			// Remove the last trailing +, because I'm too lazy not to add it
			output.pop_back();
			output.pop_back();
		} else {
			output += "0 ";
		}
		if (reaction.rate != 1) {
			output += "->";
			output += "(";
			output += precision::to_string(reaction.rate);
			output += ") ";
		} else {
			output += "-> ";
		}
		const auto &products = reaction.products;
		if (!products.empty()) {
			for (const auto &specie : reaction.products) {
				if (name != "main") {
					output += name + "_" + specie.first + " + ";
				} else {
					output += specie.first + " + ";
				}
			}
			output.pop_back();
			output.pop_back();
			output.pop_back();
		} else {
			output += "0";
		}
		output += ";\n";
	}
	return output;
}

void Module::Verify() {
	std::set<specie> declaredSpecies;
	for (const auto &specie : inputSpecies) {
		declaredSpecies.insert(specie);
	}
	for (const auto &specie : outputSpecies) {
		declaredSpecies.insert(specie);
	}
	for (const auto &specie : privateSpecies) {
		declaredSpecies.insert(specie);
	}

	for (const auto &c : concentrations) {
		if (declaredSpecies.find(c.first) == declaredSpecies.end()) {
			throw SpecieNotDeclaredException(c.first, name);
		}
		for (const auto &specie : inputSpecies) {
			if (c.first == specie) {
				throw InputSpecieConcException(c.first, name);
			}
		}
	}
	for (const auto &reaction : reactions) {
		for (const auto &specie : reaction.reactants) {
			if (declaredSpecies.find(specie.first) == declaredSpecies.end()) {
				throw SpecieNotDeclaredException(specie.first, name);
			}
		}
		for (const auto &specie : reaction.products) {
			if (declaredSpecies.find(specie.first) == declaredSpecies.end()) {
				throw SpecieNotDeclaredException(specie.first, name);
			}
		}
	}
}

void Module::ApplyCompositions() {
	int compositionNumber = 0;
	while (!compositions.empty()) {
		Composition *comp = compositions.back();
		comp->ApplyComposition(name, compositionNumber++, concentrations, reactions,
													 privateSpecies);
		compositions.pop_back();
	}
}

void Module::VerifyFunction() {
	for (const auto &input : inputSpecies) {
		for (auto reaction : reactions) {
			if (reaction.reactants[input] != reaction.products[input])
				throw FunctionIncorrectReactionsException(name);
		}
	}
}
std::string Module::MapName(std::string moduleName, std::string specieName,
														int specieNumber) {
	std::string output;
	if (moduleName != "main") {
		if (specieNumber != 1) {

			output +=
					std::to_string(specieNumber) + moduleName + "_" + specieName + " + ";
		} else {
			output += moduleName + "_" + specieName + " + ";
		}
	} else {
		if (specieNumber != 1) {
			output += std::to_string(specieNumber) + specieName + " + ";
		} else {
			output += specieName + " + ";
		}
	}
	return output;
}
