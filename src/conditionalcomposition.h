#pragma once
#include "composition.h"

/*! \brief A composition in which the reactions have a catalyst
 * \detail This type of composition allows for the user to have their
 * composition in a format `if (specie) { compositions }`, where all
 * compositions will have `specie` as a catalyst
 */
class ConditionalComposition : public Composition {
public:
	/** Constructor
	 *
	 * @param condition The specie which is added as acatalyst
	 * @param subCompositions The child compositions
	 */
	ConditionalComposition(specie condition,
												 std::vector<Composition *> subCompositions)
			: condition(condition), subCompositions(subCompositions) {}

	void ApplyComposition(std::string moduleName, int compositionNumber,
												std::map<specie, int> &concOut,
												std::vector<reaction> &reactionOut,
												std::vector<specie> &specieOut) override;

private:
	specie condition;
	std::vector<Composition *> subCompositions;
};
