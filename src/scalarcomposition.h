#pragma once
#include "composition.h"

class ScalarComposition : public Composition {
public:
	ScalarComposition(double time, double concentration,
										std::vector<Composition *> subCompositions) {
		time = time;
		concentration = concentration;
	}

	void ApplyComposition(std::string moduleName, int compositionNumber,
												std::map<specie, int> &concOut,
												std::vector<reaction> &reactionOut,
												std::vector<specie> &specieOut) override;
private:
	double time;
	double concentration;
	std::vector<Composition *> subCompositions;
};
