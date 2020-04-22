#include "composition.h"
#include "module.h"

Composition::Composition(Module *module, std::vector<specie> inputs,
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

/*
void Composition::ApplyComposition(std::string moduleName,
															std::map<specie, int> &concOut,
															std::vector<reaction> &reactionOut,
															std::vector<specie> &specieOut) {}*/
