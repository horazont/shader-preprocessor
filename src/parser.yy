%{

#include "spp/context.hpp"
#include "spp/lexer.hpp"

%}

%require "2.3"
%debug
%start start
%defines
%skeleton "lalr1.cc"
%name-prefix "spp"
%define "parser_class_name" {Parser}
%locations

%parse-param { spp::ParserContext &ctx }
%error-verbose

%union {
    std::string *sourceline;
}

%token END 0 "end of file"
%token EOL "end of line"
%token <sourceline> SOURCELINE "shader source line"

%type <sourceline> program start

%destructor { delete $$; } SOURCELINE program start

%{

#undef yylex
#define yylex ctx.lexer().lex

%}

%%

program
    : program SOURCELINE
    {
        $$ = $1;
        (*$$) += *$2;
        delete $2;
    }
    | SOURCELINE
    {
        $$ = $1;
    }

start: program
    {
        std::cout << *$1 << std::endl;
        $$ = $1;
    }

%%

void spp::Parser::error(const spp::Parser::location_type &l,
                        const std::string &m)
{
    ctx.error(l, m);
}
