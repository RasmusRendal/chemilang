#pragma once
#include <map>
#include <string>

#define MAX_DECS 10

using specie = std::string;
using speciesRatios = std::map<specie, int>;
using reactionRate = double;

struct reaction {
	speciesRatios reactants;
	speciesRatios products;
	reactionRate rate;
};

using speciesMapping = std::map<specie, specie>;
