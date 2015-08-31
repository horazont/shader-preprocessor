%{

#include "spp/context.hpp"
#include "spp/lexer.hpp"

#include <unordered_map>

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
    std::string *sourcecode;
    std::string *strlit;
    long int intlit;
    Program *program;
    VersionDeclaration *version;
    IncludeDirective *include;
}

%token END 0 "end of file"
%token EOL "end of line"
%token VERSION "#version directive"
%token <sourcecode> SOURCECODE "shader source"
%token INTLIT "integer literal"
%token STRLIT "string literal"
%token IDENT "identifier"
%token INCLUDE "#include directive"
%token DIROPEN "start of directive"
%token DIREND "end of directive"
%token ERROR

%type <program> program
%type <version> version
%type <include> include
%type <strlit> STRLIT IDENT ERROR strlit
%type <intlit> shader_type INTLIT

%destructor { delete $$; } SOURCECODE IDENT
%destructor { delete $$; } version

%{

#undef yylex
#define yylex ctx.lexer().lex

%}

%%

shader_type
    : IDENT
    {
        static const std::unordered_map<std::string, ProgramType> mapping({
            std::make_pair("fragment", ProgramType::FRAGMENT),
            std::make_pair("vertex", ProgramType::VERTEX),
            std::make_pair("tesselation", ProgramType::TESSELATION),
            std::make_pair("geometry", ProgramType::GEOMETRY),
        });
        auto it = mapping.find(*$1);
        if (it == mapping.end()) {
            error(@1, "unknown program type: " + *$1);
            $$ = static_cast<int>(ProgramType::GENERIC);
        } else {
            $$ = static_cast<int>(it->second);
        }
    }

version
    : VERSION INTLIT IDENT shader_type EOL
    {
        $$ = new VersionDeclaration($2, *$3, static_cast<ProgramType>($4));
    }
    | VERSION INTLIT IDENT EOL
    {
        $$ = new VersionDeclaration($2, *$3, ProgramType::GENERIC);
    }

strlit
    : STRLIT
    {
        $$ = $1;
    }
    | ERROR
    {
        std::string local_copy(*$1);
        delete $1;
        throw syntax_error(@1, local_copy);
    }

include
    : INCLUDE strlit EOL
    {
        $$ = new IncludeDirective(*$2);
        delete $2;
    }

program
    : program SOURCECODE
    {
        $$ = $1;
        $$->append_section(std::make_unique<StaticSourceSection>(*$2));
        delete $2;
    }
    | program include
    {
        $$ = $1;
        $$->append_section(std::unique_ptr<IncludeDirective>($2));
    }
    | program error
    {
        $$ = $1;
    }
    | version
    {
        $$ = &dest;
        $$->append_section(std::unique_ptr<VersionDeclaration>($1));
        $$->set_type($1->type());
    }

%%

void spp::Parser::error(const spp::Parser::location_type &l,
                        const std::string &m)
{
    ctx.error(l, m);
}
