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
	speciesMapping mapPri;
	module->Verify();
	module->ApplyCompositions();

	for (auto priSpecie : module->privateSpecies) {
		specie newSpecie = module->name + "_" + std::to_string(compositionNumber) +
											 "_" + priSpecie;

		privateSpecieOut.push_back(newSpecie);
		mapPri.insert(std::make_pair(priSpecie, newSpecie));
	}

	for (const auto &reaction : module->reactions) {
		reactionsOut.push_back(MapReaction(mapPri, reaction));
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

reaction ModuleComposition::MapReaction(const speciesMapping &mapPri,
																				const reaction &r) {
	speciesRatios leftSide;
	for (const auto &specie : r.reactants) {
		const std::string &specieName = specie.first;
		if (inputMapping.find(specieName) != inputMapping.end()) {
			leftSide.insert(
					std::make_pair(inputMapping.at(specieName), specie.second));
		} else if (outputMapping.find(specieName) != outputMapping.end()) {
			leftSide.insert(
					std::make_pair(outputMapping.at(specieName), specie.second));
		} else if (mapPri.find(specieName) != mapPri.end()) {
			leftSide.insert(std::make_pair(mapPri.at(specieName), specie.second));
		} else {
			throw std::runtime_error("Specie not found");
		}
	}

	speciesRatios rightSide;
	for (const auto &specie : r.products) {
		const std::string &specieName = specie.first;
		if (inputMapping.find(specieName) != inputMapping.end()) {
			rightSide.insert(
					std::make_pair(inputMapping.at(specieName), specie.second));
		} else if (outputMapping.find(specieName) != outputMapping.end()) {
			rightSide.insert(
					std::make_pair(outputMapping.at(specieName), specie.second));
		} else if (mapPri.find(specieName) != mapPri.end()) {
			leftSide.insert(std::make_pair(mapPri.at(specieName), specie.second));
		} else {
			throw std::runtime_error("Specie not found");
		}
	}

	reactionRate rate = r.rate;
	reaction mapped = {leftSide, rightSide, rate};
	return mapped;
}
