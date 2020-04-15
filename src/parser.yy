%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.4.2"
%defines

%define api.token.constructor
%define api.value.type variant
%define parse.assert

%code requires {
  # include <string>
  # include <map>
  # include <vector>
  # include <exception>
  class driver;
}

// The parsing context.
%param { driver& drv }

%locations

%define parse.trace
%define parse.error verbose

%code {
# include "driver.h"
using SpeciesPair = std::pair<std::string, int>;
using SpeciesList = std::map<std::string, int>;

void InsertToSpecieMap(SpeciesList &list, SpeciesPair &toInsert) {
	if (list.find(toInsert.first) == list.end()) {
		list.insert(toInsert);
	} else {
		list[toInsert.first] += toInsert.second;
	}
  }
}

%define api.token.prefix {TOK_}
%token
    END  0          "end of file"
    T_DMODULE       "module"
    T_DPRIVATE      "private:"
    T_DINPUT      "input:"
    T_DOUTPUT      "output:"
    T_DREACTIONS      "reactions:"
    T_DCONCENTRATIONS      "concentrations:"
    T_RIGHTARROW    "->"
    T_BIARROW       "<->"
    T_BRACKETSTART  "["
    T_BRACKETEND    "]"
    T_BRACESTART    "{"
    T_BRACEEND      "}"
    T_PARENSTART    "("
    T_PARENEND      ")"
    T_SET           ":="
    T_PLUS          "+"
    T_COMMA         ","
    T_END           ";"
;

%token <std::string>    T_NAME      "name"
%token <int>            T_NUMBER    "number"

%%

%start modules;

modules  : module
         | modules module
         ;

module : "module:" "name" "{" properties "}" { std::cout << $2 << std::endl; }
	   ;

properties : property
		   | properties property
		   ;

property : "private:" dSpecies ";"
		 | "output:" dSpecies ";"
		 | "reactions:" "{" reactions "}"
		 ;

reactions: reaction
		 | reactions reaction
		 ;

reaction: reactionSpeciesList "->" reactionSpeciesList ";"
		;

reactionSpeciesList: "name"
				   | reactionSpeciesList "+" "name"
				   ;

dSpecies: "name"
		| "[" speciesArray "]"
		;

speciesArray: "name"
			| speciesArray "," "name"
			;

%%

void yy::parser::error (const location_type&, const std::string&)
{
  //std::cerr << l << ": " << m << '\n';
}
