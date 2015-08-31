%{

#include "spp/lexer.hpp"

typedef spp::Parser::token token;
typedef spp::Parser::token_type token_type;

#define yyterminate() return token::END;

#define YY_NO_UNISTD_H

#include "spp/context.hpp"

%}

%option c++
%option prefix="spp"
%option batch
%option debug
%option yywrap nounput
%option stack

%x DIRECTIVE INSIDE_LINE

%{
#define MAX_INCLUDE_DEPTH 10
#define YY_USER_ACTION yylloc->columns(yyleng);
%}

%%

%{
  yylloc->step();
%}

<INITIAL>#version {
    BEGIN(DIRECTIVE);
    yylloc->step();
    return token::VERSION;
}

<INITIAL>#include {
    BEGIN(DIRECTIVE);
    yylloc->step();
    return token::INCLUDE;
}

<INSIDE_LINE>[^\n]*\n {
    BEGIN(INITIAL);
    yylloc->lines(1);
    yylval->sourcecode = new std::string(yytext, yyleng);
    return token::SOURCECODE;
}

<INSIDE_LINE>[^\n]+ {
    BEGIN(INITIAL);
    yylloc->lines(1);
    yylval->sourcecode = new std::string(yytext, yyleng);
    return token::SOURCECODE;
}

<DIRECTIVE>[ \t\r]+ {
    yylloc->step();
}

<DIRECTIVE>\n {
    BEGIN(INITIAL);
    yylloc->step();
    yylloc->lines(1);
    return token::EOL;
}

<DIRECTIVE>[0-9]+ {
    yylloc->step();
    yylval->intlit = atoi(yytext);
    return token::INTLIT;
}

<DIRECTIVE>[_a-zA-Z][_a-zA-Z0-9]* {
    yylloc->step();
    yylval->strlit = new std::string(yytext, yyleng);
    return token::IDENT;
}

<DIRECTIVE>\"(\\.|[^"])*\" {
    yylloc->step();
    std::string s(yytext, yyleng);
    s.erase(s.begin());
    s.erase(s.end()-1);

    bool success;
    std::tie(success, s) = spp::unescape(s);

    if (!success) {
        yylval->strlit = new std::string("invalid escape sequence in string literal");
        return token::ERROR;
    }

    yylval->strlit = new std::string(s);
    return token::STRLIT;
}

<DIRECTIVE><<EOF>> {
    BEGIN(INITIAL);
    return token::EOL;
}

<DIRECTIVE>. {
    yylloc->step();
    return static_cast<token_type>(*yytext);
}

<INITIAL>\n {
    yylval->sourcecode = new std::string(yytext, yyleng);
    yylloc->lines(1);
    return token::SOURCECODE;
}

<INITIAL>. {
    BEGIN(INSIDE_LINE);
    yylval->sourcecode = new std::string(yytext, yyleng);
    return token::SOURCECODE;
}

<<EOF>> {
    yyterminate();
}

%%

namespace spp {

Scanner::Scanner(ParserContext &context, std::istream *in, std::ostream *out):
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
