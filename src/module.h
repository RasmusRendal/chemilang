#pragma once
#include <map>
#include <set>
#include <string>
#include <tuple>
#include <vector>

class Module;

// TODO: Consider whether these would be more readable as structs
using specie = std::string;
using speciesRatios = std::map<specie, int>;
using reactionRate = int;
using speciesMapping = std::map<specie, specie>;

struct reaction {
	speciesRatios reactants;
	speciesRatios products;
	reactionRate rate;
};

struct composition {
	Module *module;
	speciesMapping inputMapping;
	speciesMapping outputMapping;
};

struct SpecieNotDeclaredException : public std::exception {
	std::string error;
	SpecieNotDeclaredException(std::string speciesName, std::string moduleName)
			: error("The species " + speciesName + " was not declared in module " +
							moduleName) {}
	const char *what() const throw() {
		return error.c_str();
	}
};

struct InputSpecieConcException : public std::exception {
	std::string error;
	InputSpecieConcException(std::string speciesName, std::string moduleName)
			: error("Illegal use of input species " + speciesName + ", in module " +
							moduleName + ". Cannot set concentration of input species") {}
	const char *what() const throw() {
		return error.c_str();
	}
};

class Module {
public:
	Module() {}
	void Verify();
	std::string Compile();
	/**
	 * Remove all compositions from the vector, and add items to the object
	 *
	 * During compilations, compositions are added as items in the compositions
	 * vector. Then, after this is done, this function should be called, and each
	 * submodules reactions, compositions and private species should be added to
	 * the supermodule. This function does that action, and pops the modules off
	 * the vector
	 */
	void ApplyCompositions();
	void InsertConcentrations(std::string moduleName, std::string specieName,
														int concentration);
	std::string name;
	std::vector<specie> inputSpecies;
	std::vector<specie> outputSpecies;
	std::vector<specie> privateSpecies;
	std::map<specie, int> concentrations;
	std::vector<reaction> reactions;
	std::vector<composition> compositions;

private:
	void MapReaction(const speciesMapping &mapIn, const speciesMapping &mapOut,
									 const reaction &r);
};
