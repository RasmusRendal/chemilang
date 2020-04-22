#include "conditionalcomposition.h"

void ConditionalComposition::ApplyComposition(
		std::string moduleName, int compositionNumber,
		std::map<specie, int> &concOut, std::vector<reaction> &reactionOut,
		std::vector<specie> &specieOut) {
	std::vector<reaction> intermediaryReactions;
	for (Composition *subcomp : subCompositions) {
		subcomp->ApplyComposition(moduleName, compositionNumber, concOut,
															intermediaryReactions, specieOut);
	}
	for (auto rcn : intermediaryReactions) {
		rcn.reactants.insert(std::make_pair(condition, 1));
		rcn.products.insert(std::make_pair(condition, 1));
		reactionOut.push_back(rcn);
	}
}
