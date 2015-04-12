#ifndef SPP_CONTEXT_H
#define SPP_CONTEXT_H

#include <string>

#include "spp/lexer.hpp"
#include "spp/ast.hpp"

/**
 * This namespace holds the Shader Preprocessor interface and implementation.
 */
namespace spp {

class location;

/**
 * Context for a Shader Preprocessor parser.
 */
class ParserContext
{
public:
    ParserContext(std::istream &in);
    virtual ~ParserContext();

private:
    std::istream &m_in;

    Scanner m_scanner;

public:
    inline Scanner &lexer()
    {
        return m_scanner;
    }

    inline const Scanner &lexer() const
    {
        return m_scanner;
    }

    virtual void error(const std::string &message);
    virtual void error(const location &loc, const std::string &message);

    std::unique_ptr<Program> parse();

};

}

#endif
