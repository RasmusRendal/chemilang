#include "module.h"
#include "composition.h"
#include "typedefs.h"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace constants {
std::string MAIN_MODULE = "main";
} // namespace constants

namespace precision {

// TODO: This does not seem ideal, but getting floats in just the right format
// seems to be a hard problem.
std::string to_string(double d) {
	std::ostringstream ss;
	ss << std::fixed;
	ss << std::setprecision(MAX_DECS);
	ss << d;
	std::string out = ss.str();
	while (out.back() == '0' || out.back() == '.') {
		out.pop_back();
	}
	return out;
}
} // namespace precision

std::string Module::Compile() {
	Verify();
	ApplyCompositions();

	std::string output;

	if (outputSpecies.size() > 0) {
		output += "-C ";
		for (const auto &specie : outputSpecies) {
			output += OutputSpecieToString(specie);
		}
		output.pop_back();
	}
	output += "\n";

	for (const auto &concs : concentrations) {
		output += SpecieConcsTostring(concs);
	}
	for (const auto &reaction : reactions) {
		if (!reaction.reactants.empty()) {
			for (const auto &specie : reaction.reactants) {
				output += SpecieReactToString(specie);
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
				output += SpecieReactToString(specie);
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

std::string
Module::SpecieReactToString(const std::pair<std::string, int> &specie) {
	return SpecieCoefToString(specie.second) + specie.first + " + ";
}

std::string Module::OutputSpecieToString(const std::string &specieName) {
	return specieName + ",";
}

std::string Module::SpecieCoefToString(int coeff) {
	return (coeff != 1) ? std::to_string(coeff) : "";
}

std::string
Module::SpecieConcsTostring(const std::pair<std::string, int> &concs) {
	return concs.first + " := " + std::to_string(concs.second) + ";\n";
}
