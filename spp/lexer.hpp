#ifndef SPP_LEXER_H
#define SPP_LEXER_H

#ifndef YY_DECL
#define YY_DECL \
    spp::Parser::token_type spp::Scanner::lex(\
        spp::Parser::semantic_type *yylval, \
        spp::Parser::location_type *yylloc)
#endif

#ifndef __FLEX_LEXER_H
#define yyFlexLexer sppFlexLexer
#include "FlexLexer.h"
#undef yyFlexLexer
#endif

namespace spp {

class ParserContext;

}

#include "spp/ast.hpp"
#include "parser.hpp"

namespace spp {

class Scanner: ::sppFlexLexer
{
public:
    Scanner(ParserContext &context,
            std::istream *arg_yyin,
            std::ostream *arg_yyout);

    ~Scanner() override;

    virtual Parser::token_type lex(
            Parser::semantic_type *yylval,
            Parser::location_type *yylloc);

    void set_debug(bool debug);
};

}

#endif
