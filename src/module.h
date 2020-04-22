#pragma once
#include "typedefs.h"
#include <map>
#include <set>
#include <string>
#include <tuple>
#include <vector>

class Module;
class Composition;

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

struct MapConcForSubModuleException : public std::exception {
	std::string error;
	MapConcForSubModuleException(std::string speciesName,
															 std::string moduleNameFrom,
															 std::string moduleNameTo)
			: error("Something went wrong, couldn't map specie: " + speciesName +
							" from module " + moduleNameFrom + " to module " + moduleNameTo +
							"'s concentrations") {}
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

	std::string name;
	std::vector<specie> inputSpecies;
	std::vector<specie> outputSpecies;
	std::vector<specie> privateSpecies;
	std::map<specie, int> concentrations;
	std::vector<reaction> reactions;
	// TODO: This should be an unique pointer instead
	std::vector<Composition *> compositions;
};
