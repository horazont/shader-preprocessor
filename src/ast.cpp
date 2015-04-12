#include "spp/ast.hpp"

namespace spp {


Section::Section()
{

}

Section::~Section()
{

}


StaticSourceSection::StaticSourceSection(const std::string &source):
    m_source(source)
{

}

void StaticSourceSection::evaluate(std::ostream &into)
{
    into << m_source;
}


Program::Program():
    m_type(Type::VERTEX)
{

}

void Program::append_section(Section *sec)
{
    m_sections.emplace_back(sec);
}

void Program::evaluate(std::ostream &dest)
{
    for (auto &section: m_sections)
    {
        section->evaluate(dest);
    }
}


}
