#include "module.h"
#include <iostream>

std::string Module::Compile() {
	Verify();
	ApplyCompositions();
	std::string output = "";
	for (const auto &c : concentrations) {
		output += name + "_" + c.first + " := " + std::to_string(c.second) + ";\n";
	}
	for (const auto &reaction : reactions) {
		for (const auto &specie : std::get<0>(reaction)) {
			output += name + "_" + specie.first + " + ";
		}
		// Remove the last trailing +, because I'm too lazy not to add it
		output.pop_back();
		output.pop_back();
		if (std::get<2>(reaction) != 1) {
			output += "->";
			output += "(";
			output += std::to_string(std::get<2>(reaction));
			output += ") ";
		} else
			output += "-> ";
		auto &products = std::get<1>(reaction);
		if (!products.empty()) {
			for (const auto &specie : std::get<1>(reaction)) {
				output += name + "_" + specie.first + " + ";
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
		for (const auto &specie : std::get<0>(reaction)) {
			if (declaredSpecies.find(specie.first) == declaredSpecies.end()) {
				throw SpecieNotDeclaredException(specie.first, name);
			}
		}
		for (const auto &specie : std::get<1>(reaction)) {
			if (declaredSpecies.find(specie.first) == declaredSpecies.end()) {
				throw SpecieNotDeclaredException(specie.first, name);
			}
		}
	}
}

void Module::MapReaction(const speciesMapping &mapIn,
												 const speciesMapping &mapOut, const reaction &r) {
	speciesRatios leftSide;
	for (const auto &specie : std::get<0>(r)) {
		const std::string &specieName = specie.first;
		if (mapIn.find(specieName) != mapIn.end()) {
			leftSide.insert(std::make_pair(mapIn.at(specieName), specie.second));
		} else if (mapOut.find(specieName) != mapOut.end()) {
			leftSide.insert(std::make_pair(mapOut.at(specieName), specie.second));
		} else {
			throw std::runtime_error("Specie not found");
		}
	}

	speciesRatios rightSide;
	for (const auto &specie : std::get<1>(r)) {
		const std::string &specieName = specie.first;
		if (mapIn.find(specieName) != mapIn.end()) {
			rightSide.insert(std::make_pair(mapIn.at(specieName), specie.second));
		} else if (mapOut.find(specieName) != mapOut.end()) {
			rightSide.insert(std::make_pair(mapOut.at(specieName), specie.second));
		} else {
			throw std::runtime_error("Specie not found");
		}
	}

	reactionRate rate = std::get<2>(r);
	reaction mapped(leftSide, rightSide, rate);
	reactions.push_back(mapped);
}

void Module::ApplyCompositions() {
	int compositionNumber = 0;
	while (!compositions.empty()) {
		composition comp = compositions.back();
		speciesMapping mapIn = std::get<1>(comp);
		speciesMapping mapOut = std::get<2>(comp);
		Module *submodule = std::get<0>(comp);
		submodule->Verify();
		submodule->ApplyCompositions();

		for (const auto &reaction : submodule->reactions) {
			MapReaction(mapIn, mapOut, reaction);
		}

		for (const auto &c : submodule->concentrations) {
			InsertConcentrations(submodule->name, c.first, c.second);
		}

		compositions.pop_back();
		compositionNumber++;
	}
}

void Module::InsertConcentrations(std::string moduleName,
																	std::string specieName, int concentration) {
	std::string newName = moduleName + "_" + specieName;
	concentrations.insert(std::pair<specie, int>(newName, concentration));
}
