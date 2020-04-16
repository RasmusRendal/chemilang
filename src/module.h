#pragma once
#include <map>
#include <set>
#include <string>
#include <tuple>
#include <vector>

using specie = std::string;
using speciesRatios = std::map<specie, int>;
using reactionRate = int;
using reaction = std::tuple<speciesRatios, speciesRatios, reactionRate>;

struct SpecieNotDeclaredException : public std::exception {
	std::string error;
	SpecieNotDeclaredException(std::string speciesName, std::string moduleName)
			: error("The species " + speciesName + " was not declared in module " +
							moduleName) {}
	const char *what() const throw() {
		return error.c_str();
	}
};

class Module {
public:
	Module() {}
	void Verify();
	std::string Compile();

	std::string name;
	std::vector<specie> inputSpecies;
	std::vector<specie> outputSpecies;
	std::vector<specie> privateSpecies;
	std::map<specie, int> concentrations;
	std::vector<reaction> reactions;
};
