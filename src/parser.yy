%{

#include "spp/context.hpp"
#include "spp/lexer.hpp"

%}

%require "2.3"
%debug
%start program
%defines
%skeleton "lalr1.cc"
%name-prefix "spp"
%define "parser_class_name" {Parser}
%locations

%parse-param { spp::ParserContext &ctx }
%parse-param { spp::Program &dest }
%error-verbose

%union {
    std::string *sourceline;
    Program *program;
}

%token END 0 "end of file"
%token EOL "end of line"
%token <sourceline> SOURCELINE "shader source line"

%type <program> program

%destructor { delete $$; } SOURCELINE

%{

#undef yylex
#define yylex ctx.lexer().lex

%}

%%

program
    : program SOURCELINE
    {
        $$ = &dest;
        $$->append_section(new StaticSourceSection(*$2));
        delete $2;
    }
    | SOURCELINE
    {
        $$ = &dest;
        $$->append_section(new StaticSourceSection(*$1));
        delete $1;
    }

%%

void spp::Parser::error(const spp::Parser::location_type &l,
                        const std::string &m)
{
    ctx.error(l, m);
}
