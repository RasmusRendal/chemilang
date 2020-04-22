%{
# include <cerrno>
# include <climits>
# include <cstdlib>
# include <cstring> // strerror
# include <string>
# include "driver.h"
# include "parser.hpp"
%}

%{
#if defined __clang__
# define CLANG_VERSION (__clang_major__ * 100 + __clang_minor__)
#endif

// Clang and ICC like to pretend they are GCC.
#if defined __GNUC__ && !defined __clang__ && !defined __ICC
# define GCC_VERSION (__GNUC__ * 100 + __GNUC_MINOR__)
#endif

// Pacify warnings in yy_init_buffer (observed with Flex 2.6.4)
// and GCC 6.4.0, 7.3.0 with -O3.
#if defined GCC_VERSION && 600 <= GCC_VERSION
# pragma GCC diagnostic ignored "-Wnull-dereference"
#endif

// This example uses Flex's C backend, yet compiles it as C++.
// So expect warnings about C style casts and NULL.
#if defined CLANG_VERSION && 500 <= CLANG_VERSION
# pragma clang diagnostic ignored "-Wold-style-cast"
# pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
#elif defined GCC_VERSION && 407 <= GCC_VERSION
# pragma GCC diagnostic ignored "-Wold-style-cast"
# pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif

#define FLEX_VERSION (YY_FLEX_MAJOR_VERSION * 100 + YY_FLEX_MINOR_VERSION)

// Old versions of Flex (2.5.35) generate an incomplete documentation comment.
//
//  In file included from src/scan-code-c.c:3:
//  src/scan-code.c:2198:21: error: empty paragraph passed to '@param' command
//        [-Werror,-Wdocumentation]
//   * @param line_number
//     ~~~~~~~~~~~~~~~~~^
//  1 error generated.
#if FLEX_VERSION < 206 && defined CLANG_VERSION
# pragma clang diagnostic ignored "-Wdocumentation"
#endif

// Old versions of Flex (2.5.35) use 'register'.  Warnings introduced in
// GCC 7 and Clang 6.
#if FLEX_VERSION < 206
# if defined CLANG_VERSION && 600 <= CLANG_VERSION
#  pragma clang diagnostic ignored "-Wdeprecated-register"
# elif defined GCC_VERSION && 700 <= GCC_VERSION
#  pragma GCC diagnostic ignored "-Wregister"
# endif
#endif

#if FLEX_VERSION < 206
# if defined CLANG_VERSION
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wdocumentation"
#  pragma clang diagnostic ignored "-Wshorten-64-to-32"
#  pragma clang diagnostic ignored "-Wsign-conversion"
# elif defined GCC_VERSION
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wsign-conversion"
# endif
#endif
%}

%option noyywrap nounput noinput batch debug

%{
  // A number symbol corresponding to the value in S.
  yy::parser::symbol_type
  make_T_NUMBER (const std::string &s, const yy::parser::location_type& loc);
%}

int               [0-9]+
T_DMODULE         "module"
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

%{
  // Code run each time a pattern is matched.
  # define YY_USER_ACTION  loc.columns (yyleng);
%}

%%
%{
  // A handy shortcut to the location held by the driver.
  yy::location& loc = drv.location;
  // Code run each time yylex is called.
  loc.step ();
%}
"#".* loc.step();
{blank}+    loc.step();
\n+         loc.lines(yyleng); loc.step();

{T_DMODULE}          return yy::parser::make_T_DMODULE         (loc);
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

void driver::scan_begin (const std::string &instream)
{
  yy_flex_debug = trace_scanning;
  yy_scan_string(instream.c_str());
}

void driver::scan_end ()
{
}
