#include "module.h"
#include <algorithm>
#include <iostream>

std::string Module::Compile() {
	Verify();
	ApplyCompositions();
	std::string output = "";
	for (const auto &c : concentrations) {
		output += name + "_" + c.first + " := " + std::to_string(c.second) + ";\n";
	}
	for (const auto &reaction : reactions) {
		for (const auto &specie : reaction.reactants) {
			if (specie.second != 1) {
				output +=
						std::to_string(specie.second) + name + "_" + specie.first + " + ";
			} else
				output += name + "_" + specie.first + " + ";
		}
		// Remove the last trailing +, because I'm too lazy not to add it
		output.pop_back();
		output.pop_back();
		if (reaction.rate != 1) {
			output += "->";
			output += "(";
			output += std::to_string(reaction.rate);
			output += ") ";
		} else
			output += "-> ";
		auto &products = reaction.products;
		if (!products.empty()) {
			for (const auto &specie : reaction.products) {
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

void Module::MapReaction(const speciesMapping &mapIn,
												 const speciesMapping &mapOut,
												 const speciesMapping &mapPri, const reaction &r) {
	speciesRatios leftSide;
	for (const auto &specie : r.reactants) {
		const std::string &specieName = specie.first;
		if (mapIn.find(specieName) != mapIn.end()) {
			std::cout << specie.first << " Is in\n";
			leftSide.insert(std::make_pair(mapIn.at(specieName), specie.second));
		} else if (mapOut.find(specieName) != mapOut.end()) {
			std::cout << specie.first << " Is out\n";
			leftSide.insert(std::make_pair(mapOut.at(specieName), specie.second));
		} else if (mapPri.find(specieName) != mapPri.end()) {
			std::cout << specie.first << " Is pri\n";
			leftSide.insert(std::make_pair(mapPri.at(specieName), specie.second));
		} else {
			throw std::runtime_error("Specie not found");
		}
	}

	speciesRatios rightSide;
	for (const auto &specie : r.products) {
		const std::string &specieName = specie.first;
		if (mapIn.find(specieName) != mapIn.end()) {
			rightSide.insert(std::make_pair(mapIn.at(specieName), specie.second));
		} else if (mapOut.find(specieName) != mapOut.end()) {
			rightSide.insert(std::make_pair(mapOut.at(specieName), specie.second));
		} else {
			throw std::runtime_error("Specie not found");
		}
	}

	reactionRate rate = r.rate;
	reaction mapped = {leftSide, rightSide, rate};
	reactions.push_back(mapped);
}

void Module::ApplyCompositions() {
	int compositionNumber = 0;
	int subIt = 1;
	while (!compositions.empty()) {
		composition comp = compositions.back();
		speciesMapping mapIn = comp.inputMapping;
		speciesMapping mapOut = comp.outputMapping;
		speciesMapping mapPri;
		Module *submodule = comp.module;
		submodule->Verify();
		submodule->ApplyCompositions();

		for (auto priSpecie : submodule->privateSpecies) {
			specie newSpecie =
					submodule->name + "_" + std::to_string(subIt) + "_" + priSpecie;

			privateSpecies.push_back(newSpecie);
			mapPri.insert(std::make_pair(priSpecie, newSpecie));
		}

		for (const auto &reaction : submodule->reactions) {
			MapReaction(mapIn, mapOut, mapPri, reaction);
		}

		for (const auto &c : submodule->concentrations) {
			concentrations.insert(
					std::pair<specie, int>(mapPri.at(c.first), c.second));
		}

		compositions.pop_back();
		compositionNumber++;
		subIt++;
	}
}
