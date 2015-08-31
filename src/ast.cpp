#include "spp/ast.hpp"

#include <iostream>

#include "spp/context.hpp"


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


Section::Section(const location &location)
{

}

Section::~Section()
{

}


VersionDeclaration::VersionDeclaration(const location &location,
                                       unsigned int version,
                                       const std::string &profile,
                                       ProgramType type):
    Section(location),
    m_version(version),
    m_profile(profile),
    m_type(type)
{

}

std::unique_ptr<Section> VersionDeclaration::copy() const
{
    return std::make_unique<VersionDeclaration>(m_location,
                                                m_version,
                                                m_profile,
                                                m_type);
}

void VersionDeclaration::evaluate(std::ostream &into, EvaluationContext &ctx)
{
    into << "#version " << m_version << " " << m_profile << std::endl;
    for (auto &define: ctx.defines()) {
        into << "#define " << std::get<0>(define) << " " << std::get<1>(define) << std::endl;
    }
}


StaticSourceSection::StaticSourceSection(const location &location,
                                         const std::string &source):
    Section(location),
    m_source(source)
{

}

std::unique_ptr<Section> StaticSourceSection::copy() const
{
    return std::make_unique<StaticSourceSection>(m_location, m_source);
}

void StaticSourceSection::evaluate(std::ostream &into, EvaluationContext &ctx)
{
    into << m_source;
}


IncludeDirective::IncludeDirective(const location &location,
                                   const std::string &path):
    Section(location),
    m_path(path)
{

}

std::unique_ptr<Section> IncludeDirective::copy() const
{
    return std::make_unique<IncludeDirective>(m_location, m_path);
}

void IncludeDirective::evaluate(std::ostream &into, EvaluationContext &ctx)
{
    throw std::runtime_error("cannot evaluate IncludeDirective");
}


Program::Program(const std::string &source_path):
    m_type(ProgramType::GENERIC),
    m_source_path(source_path)
{

}

void Program::add_local_error(const location &location, const std::string &msg)
{
    m_errors.emplace_back(m_source_path, location, msg);
}

void Program::add_error(const Program::RecordedError &ref)
{
    m_errors.emplace_back(ref);
}

void Program::append_section(std::unique_ptr<Section> &&sec)
{
    m_sections.emplace_back(std::move(sec));
}

void Program::set_type(ProgramType type)
{
    m_type = type;
}

Program::iterator Program::erase(Program::iterator iter)
{
    return Program::iterator(m_sections.erase(iter.m_curr));
}

Program::iterator Program::insert(Program::iterator before, std::unique_ptr<Section> &&section)
{
    return Program::iterator(m_sections.emplace(before.m_curr, std::move(section)));
}

std::unique_ptr<Program> Program::copy() const
{
    auto result = std::make_unique<Program>();
    for (auto &section: m_sections) {
        result->append_section(std::move(section->copy()));
    }
    return std::move(result);
}

void Program::evaluate(std::ostream &into, EvaluationContext &ctx) const
{
    for (auto &section: m_sections)
    {
        section->evaluate(into, ctx);
    }
}


}
