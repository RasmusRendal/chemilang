#include "modulecomposition.h"
#include "module.h"

ModuleComposition::ModuleComposition(Module *module, std::vector<specie> inputs,
																		 std::vector<specie> outputs)
		: module(module) {

	if (inputs.size() != module->inputSpecies.size()) {
		throw CompositionException(module->name, "input", inputs.size(),
															 module->inputSpecies.size());
	}
	for (int i = 0; i < module->inputSpecies.size(); i++) {
		inputMapping.insert(std::make_pair(module->inputSpecies[i], inputs[i]));
	}

	if (outputs.size() != module->outputSpecies.size()) {
		throw CompositionException(module->name, "output", outputs.size(),
															 module->outputSpecies.size());
	}
	for (int i = 0; i < module->outputSpecies.size(); i++) {
		outputMapping.insert(std::make_pair(module->outputSpecies[i], outputs[i]));
	}
}

void ModuleComposition::ApplyComposition(
		std::string moduleName, int compositionNumber,
		std::map<specie, int> &concOut, std::vector<reaction> &reactionsOut,
		std::vector<specie> &privateSpecieOut) {
	module->Verify();
	module->ApplyCompositions();

	for (auto priSpecie : module->privateSpecies) {
		specie newSpecie = module->name + "_" + std::to_string(compositionNumber) +
											 "_" + priSpecie;

		privateSpecieOut.push_back(newSpecie);
		mapPri.insert(std::make_pair(priSpecie, newSpecie));
	}

	for (const auto &reaction : module->reactions) {
		reactionsOut.push_back(MapReaction(reaction));
	}

	for (const auto &c : module->concentrations) {
		if (mapPri.find(c.first) != mapPri.end()) {
			concOut.insert(std::pair<specie, int>(mapPri.at(c.first), c.second));
		} else if (outputMapping.find(c.first) != outputMapping.end()) {
			concOut.insert(
					std::pair<specie, int>(outputMapping.at(c.first), c.second));
		} else {
			throw MapConcForSubModuleException(c.first, module->name, moduleName);
		}
	}
}

reaction ModuleComposition::MapReaction(const reaction &r) {
	speciesRatios leftSide;
	for (const auto &specie : r.reactants) {
		const std::string &specieName = specie.first;
		leftSide.insert(std::make_pair(MapSpecie(specieName), specie.second));
	}

	speciesRatios rightSide;
	for (const auto &specie : r.products) {
		const std::string &specieName = specie.first;
		rightSide.insert(std::make_pair(MapSpecie(specieName), specie.second));
	}

	reactionRate rate = r.rate;
	reaction mapped = {leftSide, rightSide, rate};
	return mapped;
}

specie ModuleComposition::MapSpecie(const specie &specieName) {
	if (inputMapping.find(specieName) != inputMapping.end()) {
		return inputMapping.at(specieName);
	} else if (outputMapping.find(specieName) != outputMapping.end()) {
		return outputMapping.at(specieName);
	} else if (mapPri.find(specieName) != mapPri.end()) {
		return mapPri.at(specieName);
	} else {
		// To make the compiler happy
		throw std::runtime_error("Specie not found. This should never happen.");
	}
}
