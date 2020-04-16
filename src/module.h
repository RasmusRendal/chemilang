#pragma once
#include <string>
#include <vector>
#include <tuple>
#include <map>

using specie = std::string;
using speciesRatios = std::map<specie, int>;
using reactionRate = int;
using reaction = std::tuple<speciesRatios, speciesRatios, reactionRate>;

class Module {
	public:
		Module() {}
		std::string Compile();

		std::string name;
		std::vector<specie> inputSpecies;
		std::vector<specie> outputSpecies;
		std::vector<specie> species;
		std::map<specie, int> concentrations;
		std::vector<reaction> reactions;
};
