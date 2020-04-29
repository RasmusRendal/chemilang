#include "scalarcomposition.h"

void ScalarComposition::ApplyComposition(std::string moduleName,
																				 int compositionNumber,
																				 std::map<specie, int> &concOut,
																				 std::vector<reaction> &reactionOut,
																				 std::vector<specie> &specieOut) {
	for (Composition *subcomp : subCompositions) {
    subcomp->ApplyComposition(moduleName, compositionNumber,
       concOut, reactionOut, specieOut);
  }
}
