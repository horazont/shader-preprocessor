#include "spp/context.hpp"

namespace spp {

ParserContext::ParserContext(std::istream &in):
    m_in(in),
    m_scanner(&m_in, nullptr)
{

}

ParserContext::~ParserContext()
{

}

void ParserContext::error(const std::string &message)
{
    std::cerr << "parser error: " << message << std::endl;
}

void ParserContext::error(const location &loc, const std::string &message)
{
    std::cerr << "parser error: " << message << std::endl;
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
