#pragma once
#include "typedefs.h"
#include <map>
#include <vector>

class Module;

class Composition {
public:
	/*!
	 * \brief Apply the composition to the referenced properties
	 * \detail ApplyComposition takes a module name, a composition number, and the
	 * properties of a module that are to be modified by a composition. As this is
	 * an abstract class, it allows for many implementations. Currently, the only
	 * two present are Conditionals and Modules. They are, however, quite
	 * interesting.
	 */
	virtual void ApplyComposition(std::string moduleName, int compositionNumber,
																std::map<specie, int> &concOut,
																std::vector<reaction> &reactionOut,
																std::vector<specie> &specieOut) = 0;
};
