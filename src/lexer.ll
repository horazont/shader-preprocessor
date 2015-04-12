%{

#include "spp/lexer.hpp"

typedef spp::Parser::token token;
typedef spp::Parser::token_type token_type;

#define yyterminate() return token::END;

#define YY_NO_UNISTD_H

%}

%option c++
%option prefix="spp"
%option batch
%option debug
%option yywrap nounput
%option stack

%{
#define YY_USER_ACTION yylloc->columns(yyleng);
%}

%%

%{
  yylloc->step();
%}

[^\n]*\n {
  yylval->sourceline = new std::string(yytext, yyleng);
  return token::SOURCELINE;
}

. {
  return static_cast<token_type>(*yytext);
}

%%

namespace spp {

Scanner::Scanner(std::istream *in, std::ostream *out):
    sppFlexLexer(in, out)
{

}

Scanner::~Scanner()
{

}

void Scanner::set_debug(bool debug)
{
    yy_flex_debug = debug;
}

}

#ifdef yylex
#undex yylex
#endif

int sppFlexLexer::yylex()
{
    throw std::runtime_error("sppFlexLexer::yylex() called");
}

int sppFlexLexer::yywrap()
{
    return true;
}
