#include "spp/context.hpp"

#include <iomanip>
#include <sstream>

namespace spp {

ParserContext::ParserContext(std::istream &in, const std::string &source_path):
    m_in(in),
    m_source_path(source_path),
    m_scanner(*this, &m_in, nullptr),
    m_errors()
{

}

ParserContext::~ParserContext()
{

}

std::unique_ptr<Program> ParserContext::parse()
{
    auto prog = std::make_unique<Program>(m_source_path);
    Parser parser(*this, *prog);
    if (parser.parse() != 0) {
        return nullptr;
    }
    return prog;
}

Library::Library():
    Library(std::make_unique<DefaultLoader>())
{

}

Library::Library(std::unique_ptr<Loader> &&loader):
    m_max_include_depth(100),
    m_loader(std::move(loader))
{

}

Library::~Library()
{

}

void Library::resolve_includes(Program *in_program, unsigned int depth)
{
    for (auto iter = in_program->begin();
         iter != in_program->end();
         ++iter)
    {
        IncludeDirective *include = dynamic_cast<IncludeDirective*>(&(*iter));
        if (!include) {
            continue;
        }

        const Program *included = nullptr;
        try {
            included = _load(include->path(), depth);
        } catch (const std::runtime_error &err) {
            // include failed, this can be e.g. due to too deep recursion
            in_program->add_local_error(
                        include->loc(),
                        std::string("failed to load included file: ")+err.what());
            iter = in_program->erase(iter);
            if (iter == in_program->end()) {
                break;
            }
            continue;
        }

        if (!included) {
            in_program->add_local_error(include->loc(),
                                        "failed to load included file");
            iter = in_program->erase(iter);
            if (iter == in_program->end()) {
                break;
            }
            continue;
        }

        if (!included->errors().empty()) {
            for (auto &error: included->errors()) {
                in_program->add_error(error);
            }
            // include failed
            iter = in_program->erase(iter);
            if (iter == in_program->end()) {
                break;
            }
            continue;
        }

        iter = in_program->erase(iter);

        // we can safely +1 here, because a valid program always has a version
        // declaration and invalid programs have at least one error.
        for (auto included_iter = ++included->cbegin();
             included_iter != included->cend();
             ++included_iter)
        {
            iter = ++in_program->insert(iter, (*included_iter).copy());
        }

        if (iter == in_program->end()) {
            break;
        }
    }
}

const Program *Library::_load(const std::string &path, unsigned int depth)
{
    if (depth > m_max_include_depth) {
        throw std::runtime_error("maximum include depth exceeded");
    }

    {
        auto iter = m_cache.find(path);
        if (iter != m_cache.end()) {
            if (!iter->second) {
                throw std::runtime_error("recursive inclusion detected");
            }
            return iter->second.get();
        }
    }

    std::unique_ptr<std::istream> input(m_loader->open(path));
    if (!input) {
        return nullptr;
    }

    ParserContext parser(*input, path);

    // mark the file as being loaded in the cache
    m_cache[path] = nullptr;

    auto program = parser.parse();
    if (!program) {
        return nullptr;
    }


    Program *result = program.get();
    resolve_includes(result, depth+1);
    m_cache[path] = std::move(program);

    return result;
}

const Program *Library::load(const std::string &path)
{
    return _load(path, 0);
}

EvaluationContext::EvaluationContext(Library &library):
    m_library(library)
{

}

void EvaluationContext::define(const std::string &name, const std::string &rhs)
{
    {
        auto iter = m_define_names.find(name);
        if (iter != m_define_names.end()) {
            throw std::invalid_argument("duplicate define " + name);
        }
    }

    m_define_names.insert(name);
    m_defines.emplace_back(name, rhs);
}

void EvaluationContext::define1ull(const std::string &name, const unsigned long long value)
{
    define(name, std::to_string(value));
}

void EvaluationContext::define1ll(const std::string &name, const signed long long value)
{
    define(name, std::to_string(value));
}

void EvaluationContext::define1f(const std::string &name, const float value)
{
    std::ostringstream tmp;
    tmp << std::setprecision(10) << value;
    define(name, tmp.str());
}

void EvaluationContext::define1d(const std::string &name, const double value)
{
    std::ostringstream tmp;
    tmp << std::setprecision(18) << value;
    define(name, tmp.str());
}

}
