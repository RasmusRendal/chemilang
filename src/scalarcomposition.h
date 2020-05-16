#pragma once
#include "composition.h"

class ScalarComposition : public Composition {
public:
	ScalarComposition(double scale, std::vector<Composition *> subCompositions)
			: subCompositions(subCompositions), scale(scale) {
	}

	void ApplyComposition(std::string moduleName, int compositionNumber,
												std::map<specie, int> &concOut,
												std::vector<reaction> &reactionOut,
												std::vector<specie> &specieOut) override;

private:
	double scale;
	std::vector<Composition *> subCompositions;
};
