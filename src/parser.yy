%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.4.2"
%defines

%define api.token.constructor
%define api.value.type variant
%define parse.assert

%code requires {
  #include <string>
  #include <map>
  #include <vector>
  #include <exception>
  #include "module.h"
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

%nterm <speciesRatios> reactionSpeciesList

%%

%start modules;

modules  : module
         | modules module
         ;

module : T_DMODULE "name" "{" properties "}" { drv.currentModule.name = $2; drv.FinishParsingModule(); }
	   ;

properties : property
		   | properties property
		   ;

property : "private:" dSpecies ";"
		 | "output:" dSpecies ";"
		 | "reactions:" "{" reactions "}"
		 | "concentrations:" "{" concentrations "}"
		 ;

reactions: reaction
		 | reactions reaction
		 ;

reaction: reactionSpeciesList "->" reactionSpeciesList ";" { drv.currentModule.reactions.push_back(std::make_tuple($1, $3, 1)); }
		| reactionSpeciesList "->" "number" ";" { drv.currentModule.reactions.push_back(std::make_tuple($1, speciesRatios(), 1)); }
		;

reactionSpeciesList: "name" { speciesRatios l; l.insert(std::pair<specie, int>($1, 1)); $$ = l; }
				   | reactionSpeciesList "+" "name" { speciesRatios l = $1; l.insert(std::pair<specie, int>($3, 1)); $$ = l; }
				   ;

dSpecies: "name"
		| "[" speciesArray "]"
		;

speciesArray: "name"
			| speciesArray "," "name"
			;

concentrations: concentration
			  | concentrations concentration
			  ;

concentration: "name" ":=" "number" ";" {drv.currentModule.concentrations.insert(std::make_pair($1, $3));}
			 ;

%%

void yy::parser::error (const location_type &l, const std::string &m)
{
  std::cerr << l << ": " << m << '\n';
}
