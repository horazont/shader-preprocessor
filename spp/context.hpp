#ifndef SPP_CONTEXT_H
#define SPP_CONTEXT_H

#include <string>

/**
 * This namespace holds the Shader Preprocessor interface and implementation.
 */
namespace spp {

class Scanner;
class location;

/**
 * Context for a Shader Preprocessor parser.
 */
class ParserContext
{

public:
    Scanner &lexer();
    const Scanner &lexer() const;

    void error(const std::string &message);
    void error(const location &loc, const std::string &message);

};

}

#endif
