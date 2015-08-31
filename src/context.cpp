#include "spp/context.hpp"

namespace spp {

ParserContext::ParserContext(std::istream &in):
    m_in(in),
    m_scanner(*this, &m_in, nullptr),
    m_errors()
{

}

ParserContext::~ParserContext()
{

}

void ParserContext::error(const std::string &message)
{
    m_errors.emplace_back(location(), message);
    /* std::cerr << "parser error: " << message << std::endl; */
}

void ParserContext::error(const location &loc, const std::string &message)
{
    m_errors.emplace_back(loc, message);
    /* std::cerr << "parser error: " << loc.begin.line << ":" << loc.begin.column << ": " << message << std::endl; */
}

std::unique_ptr<Program> ParserContext::parse()
{
    std::unique_ptr<Program> prog(new Program());
    Parser parser(*this, *prog);
    if (parser.parse() != 0) {
        return nullptr;
    }
    return prog;
}

}
