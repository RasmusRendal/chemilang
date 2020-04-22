#pragma once
#include "composition.h"
#include "typedefs.h"
#include <map>
#include <vector>

class Module;

struct CompositionException : public std::exception {
	std::string error;

	CompositionException(std::string moduleName, std::string input, int real,
											 int expected)
			: error("Composition failed for module '" + moduleName +
							"'. It expects " + std::to_string(expected) + " " + input +
							" species, but " + std::to_string(real) + " were provided.") {}
	const char *what() const throw() {
		return error.c_str();
	}
};

class ModuleComposition : public Composition {
public:
	ModuleComposition(Module *module, std::vector<specie> inputs,
										std::vector<specie> outputs);
	ModuleComposition(Module *module, speciesMapping inputMap,
										speciesMapping outputMap)
			: module(module), inputMapping(inputMap), outputMapping(outputMap) {}
	void ApplyComposition(std::string moduleName, int compositionNumber,
												std::map<specie, int> &concOut,
												std::vector<reaction> &reactionOut,
												std::vector<specie> &specieOut) override;

	reaction MapReaction(const speciesMapping &mapPri, const reaction &r);
	speciesMapping inputMapping;
	speciesMapping outputMapping;
	Module *module;
};
