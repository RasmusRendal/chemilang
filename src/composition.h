#pragma once
#include "typedefs.h"
#include <map>
#include <vector>

class Module;

class Composition {
public:
	virtual void ApplyComposition(std::string moduleName, int compositionNumber,
																std::map<specie, int> &concOut,
																std::vector<reaction> &reactionOut,
																std::vector<specie> &specieOut) = 0;
};
