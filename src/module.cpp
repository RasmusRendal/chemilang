#include "module.h"

std::string Module::Compile() {
	std::string output = "";
	for (const auto &c : concentrations) {
		output += name + "_" + c.first + " := " + std::to_string(c.second) + ";\n";
	}
	for (const auto &reaction : reactions) {
		for (const auto &specie : std::get<0>(reaction)) {
			output += name + "_" + specie.first + " + ";
		}
		// Remove the last trailing +, because I'm too lazy not to add it
		output.pop_back();
		output.pop_back();
		output += "-> ";
		auto &products = std::get<1>(reaction);
		if (!products.empty()) {
			for (const auto &specie : std::get<1>(reaction)) {
				output += name + "_" + specie.first + " + ";
			}
			output.pop_back();
			output.pop_back();
			output.pop_back();
		} else {
			output += "0";
		}
		output += ";\n";
	}
	return output;
}
