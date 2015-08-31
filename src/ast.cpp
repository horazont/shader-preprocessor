#include "spp/ast.hpp"

#include <iostream>


namespace spp {

std::string escape(const std::string &src)
{
    std::string result(src);
    for (auto iter = result.begin(); iter != result.end(); ++iter)
    {
        switch (*iter)
        {
        case '\\':
        case '"':
        {
            iter = result.insert(iter, '\\');
            ++iter;
            break;
        }
        case '\n':
        {
            iter = result.insert(iter, '\\');
            ++iter;
            *iter = 'n';
            break;
        }
        case '\r':
        {
            iter = result.insert(iter, '\\');
            ++iter;
            *iter = 'r';
            break;
        }
        default:;
        }
    }
    return result;
}

std::tuple<bool, std::string> unescape(const std::string &src)
{
    std::string s(src);
    for (auto iter = s.begin(); iter != s.end(); ++iter)
    {
        if (*iter == '\\') {
            iter = s.erase(iter);
            // fall-through to handle the following character
            if (iter == s.end()) {
                s.insert(iter, '\\');
                // re-insert and break
                break;
            }
        } else {
            continue;
        }

        if (*iter == '\"') {
            // just do nothing
            continue;
        } else if (*iter == 'n') {
            // insert newline
            *iter = '\n';
        } else if (*iter == 'r') {
            // insert carriage return
            *iter = '\n';
        } else {
            // unknown escape, error
            return std::make_tuple(false, std::string());
        }
    }
    return std::make_tuple(true, s);
}


Section::Section()
{

}

Section::~Section()
{

}


VersionDeclaration::VersionDeclaration(unsigned int version,
                                       const std::string &profile,
                                       ProgramType type):
    m_version(version),
    m_profile(profile),
    m_type(type)
{

}

void VersionDeclaration::evaluate(std::ostream &into)
{
    into << "#version " << m_version << " " << m_profile << std::endl;
}


StaticSourceSection::StaticSourceSection(const std::string &source):
    m_source(source)
{

}

void StaticSourceSection::evaluate(std::ostream &into)
{
    into << m_source;
}


IncludeDirective::IncludeDirective(const std::string &path):
    Section(),
    m_path(path)
{

}

void IncludeDirective::evaluate(std::ostream &into)
{
    into << "#include \"" << escape(m_path) << "\"" << std::endl;
}


Program::Program():
    m_type(ProgramType::VERTEX)
{

}

void Program::append_section(std::unique_ptr<Section> &&sec)
{
    m_sections.emplace_back(std::move(sec));
}

void Program::evaluate(std::ostream &dest)
{
    for (auto &section: m_sections)
    {
        section->evaluate(dest);
    }
}

void Program::set_type(ProgramType type)
{
    m_type = type;
}


}
