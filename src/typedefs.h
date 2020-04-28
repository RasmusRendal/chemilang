#pragma once
#include <map>
#include <string>

using specie = std::string;
using speciesRatios = std::map<specie, int>;
using reactionRate = double;

struct reaction {
	speciesRatios reactants;
	speciesRatios products;
	reactionRate rate;
};

using speciesMapping = std::map<specie, specie>;
