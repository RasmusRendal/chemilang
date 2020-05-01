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
			output += MapSpecie(specie) + ",";
		}
		output.pop_back();
	}
	output += "\n";

	for (const auto &concs : concentrations) {
		output += MapSpecieConcs(concs);
	}
	for (const auto &reaction : reactions) {
		if (!reaction.reactants.empty()) {
			for (const auto &specie : reaction.reactants) {
				output += MapSpecieReact(specie);
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
				output += MapSpecieReact(specie);
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

std::string Module::MapSpecieReact(const std::pair<std::string, int> &specie) {
	return MapSpecieCoef(specie.second) + MapSpecie(specie.first) + " + ";
}

std::string Module::MapSpecieCoef(int coeff) {
	return (coeff != 1) ? std::to_string(coeff) : "";
}

std::string Module::MapSpecie(const std::string &specie) {
	return (name == constants::MAIN_MODULE) ? specie : name + "_" + specie;
}

std::string Module::MapSpecieConcs(const std::pair<std::string, int> &concs) {
	return MapSpecie(concs.first) + " := " + std::to_string(concs.second) + ";\n";
}
std::string Module::MapSpecieOut(std::string &specieName) {
	return MapSpecie(specieName) + ",";
}
