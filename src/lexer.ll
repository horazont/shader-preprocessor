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
%option yywrap
%option stack

%x VERSION_DIRECTIVE DIRECTIVE CODE

%{
#define MAX_INCLUDE_DEPTH 10
#define YY_USER_ACTION yylloc->columns(yyleng);
%}

%%

%{
  yylloc->step();
%}

<INITIAL>#version {
    BEGIN(VERSION_DIRECTIVE);
    yylloc->step();
    return token::VERSION;
}

<INITIAL>. {
    // no version directive. this will break with the parser, but we want to
    // check the remainder of the program. so we have to make reasonable output
    // here...
    unput(*yytext);
    BEGIN(CODE);
}

<VERSION_DIRECTIVE>[ \t\r]+ {
    yylloc->step();
}

<VERSION_DIRECTIVE>\n {
    BEGIN(CODE);
    yylloc->step();
    yylloc->lines(1);
    return token::EOL;
}

<VERSION_DIRECTIVE>[0-9]+ {
    yylloc->step();
    yylval->intlit = atoi(yytext);
    return token::INTLIT;
}

<VERSION_DIRECTIVE>[_a-zA-Z][_a-zA-Z0-9]* {
    yylloc->step();
    yylval->strlit = new std::string(yytext, yyleng);
    return token::IDENT;
}

<VERSION_DIRECTIVE><<EOF>> {
    BEGIN(INITIAL);
    return token::EOL;
}

<VERSION_DIRECTIVE>. {
    yylloc->step();
    return static_cast<token_type>(*yytext);
}

<CODE>\{\% {
    BEGIN(DIRECTIVE);
    yylloc->step();
    return token::DIROPEN;
}

<CODE>\{ {
    yylloc->step();
    yylval->strlit = new std::string(yytext, yyleng);
    return token::SOURCECODE;
}

<CODE>[^{\n]*\n {
    yylloc->step();
    yylloc->lines(1);
    yylval->strlit = new std::string(yytext, yyleng);
    return token::SOURCECODE;
}

<CODE>[^{\n]+ {
    yylloc->step();
    yylval->strlit = new std::string(yytext, yyleng);
    return token::SOURCECODE;
}

<DIRECTIVE>include {
    yylloc->step();
    return token::DIRECTIVE_INCLUDE;
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

<DIRECTIVE>[ \t\r]+ {
    yylloc->step();
}

<DIRECTIVE>%\} {
    BEGIN(CODE);
    return token::DIRCLOSE;
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
