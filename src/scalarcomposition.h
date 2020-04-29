#pragma once
#include "composition.h"

class ScalarComposition : public Composition {
public:
	ScalarComposition(double time,
										std::vector<Composition *> subCompositions)
									 : subCompositions(subCompositions) {
		this->time = time;
	}

	void ApplyComposition(std::string moduleName, int compositionNumber,
												std::map<specie, int> &concOut,
												std::vector<reaction> &reactionOut,
												std::vector<specie> &specieOut) override;
private:
	double time;
	std::vector<Composition *> subCompositions;
};
