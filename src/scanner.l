%{
# include <cerrno>
# include <climits>
# include <cstdlib>
# include <cstring> // strerror
# include <string>
# include "driver.h"
# include "parser.hpp"
%}

%option noyywrap nounput noinput batch debug

%{
  // A number symbol corresponding to the value in S.
  yy::parser::symbol_type
  make_T_NUMBER (const std::string &s, const yy::parser::location_type& loc);

  yy::parser::symbol_type
  make_T_DECIMAL (const std::string &s, const yy::parser::location_type& loc);
%}

int               [0-9]+
decimal           [0-9]+\.[0-9]+
T_DMODULE         "module"
T_DSCALE          "scale"
T_DFUNCTION       "function"
T_DPRIVATE        "private:"
T_DINPUT          "input:"
T_DOUTPUT         "output:"
T_DREACTIONS      "reactions:"
T_DCONCENTRATIONS "concentrations:"
T_DCOMPOSITIONS   "compositions:"
T_DIF             "if"
T_NAME            [a-zA-Z][a-zA-Z0-9]*
T_RIGHTARROW      "->"
T_BIARROW         "<->"
T_BRACKETSTART    "["
T_BRACKETEND      "]"
T_BRACESTART      "{"
T_BRACEEND        "}"
T_PARENSTART      "("
T_PARENEND        ")"
T_SET             ":="
T_EQUALS             "="
T_PLUS            "+"
T_COMMA           ","
T_END             ";"
T_NEWLINE         [\n]
blank             [ \t\r]


%%
%{
  yy::location& loc = drv.location;
%}
"#".* loc.step();
{blank}+    loc.step();
\n+         loc.lines(yyleng); loc.step();

{T_DMODULE}          return yy::parser::make_T_DMODULE         (loc);
{T_DFUNCTION}        return yy::parser::make_T_DFUNCTION       (loc);
{T_DSCALE}           return yy::parser::make_T_DSCALE          (loc);
{T_DPRIVATE}         return yy::parser::make_T_DPRIVATE        (loc);
{T_DINPUT}           return yy::parser::make_T_DINPUT          (loc);
{T_DOUTPUT}          return yy::parser::make_T_DOUTPUT         (loc);
{T_DREACTIONS}       return yy::parser::make_T_DREACTIONS      (loc);
{T_DCONCENTRATIONS}  return yy::parser::make_T_DCONCENTRATIONS (loc);
{T_DCOMPOSITIONS}    return yy::parser::make_T_DCOMPOSITIONS   (loc);
{T_DIF}              return yy::parser::make_T_DIF             (loc);
{T_RIGHTARROW}       return yy::parser::make_T_RIGHTARROW      (loc);
{T_BIARROW}          return yy::parser::make_T_BIARROW         (loc);
{T_BRACKETSTART}     return yy::parser::make_T_BRACKETSTART    (loc);
{T_BRACKETEND}       return yy::parser::make_T_BRACKETEND      (loc);
{T_BRACESTART}       return yy::parser::make_T_BRACESTART      (loc);
{T_BRACEEND}         return yy::parser::make_T_BRACEEND        (loc);
{T_PARENSTART}       return yy::parser::make_T_PARENSTART      (loc);
{T_PARENEND}         return yy::parser::make_T_PARENEND        (loc);
{T_EQUALS}           return yy::parser::make_T_EQUALS          (loc);
{T_SET}              return yy::parser::make_T_SET             (loc);
{T_PLUS}             return yy::parser::make_T_PLUS            (loc);
{T_COMMA}            return yy::parser::make_T_COMMA           (loc);
{T_END}              return yy::parser::make_T_END             (loc);

{int}                return make_T_NUMBER(yytext, loc);
{decimal}            return make_T_DECIMAL(yytext, loc);
{T_NAME}             return yy::parser::make_T_NAME (yytext, loc);
.               {
                  throw yy::parser::syntax_error
                  (loc, "invalid character: " + std::string(yytext));
                }

<<EOF>>    return yy::parser::make_END (loc);
%%

yy::parser::symbol_type
make_T_NUMBER (const std::string &s, const yy::parser::location_type& loc)
{
  errno = 0;
  long n = strtol (s.c_str(), NULL, 10);
  if (! (INT_MIN <= n && n <= INT_MAX && errno != ERANGE))
    throw yy::parser::syntax_error (loc, "integer is out of range: " + s);
  return yy::parser::make_T_NUMBER ((int) n, loc);
}

yy::parser::symbol_type
make_T_DECIMAL (const std::string &s, const yy::parser::location_type& loc)
{
  errno = 0;
  double d = atof (s.c_str());
  return yy::parser::make_T_DECIMAL ( (double) d, loc);
}

void driver::scan_begin (const std::string &instream)
{
  yy_flex_debug = trace_scanning;
  yy_scan_string(instream.c_str());
}

void driver::scan_end () {}
