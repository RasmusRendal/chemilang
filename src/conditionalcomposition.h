#pragma once
#include "composition.h"

class ConditionalComposition : public Composition {
public:
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
