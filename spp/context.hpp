#ifndef SPP_CONTEXT_H
#define SPP_CONTEXT_H

#include <unordered_map>
#include <set>
#include <string>

#include "spp/lexer.hpp"
#include "spp/ast.hpp"
#include "spp/loader.hpp"

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
    ParserContext(std::istream &in, const std::string &source_path = "<memory>");
    virtual ~ParserContext();

private:
    std::istream &m_in;
    std::string m_source_path;

    Scanner m_scanner;

protected:
    std::vector<std::tuple<location, std::string> > m_errors;

public:
    inline Scanner &lexer()
    {
        return m_scanner;
    }

    inline const Scanner &lexer() const
    {
        return m_scanner;
    }

    std::unique_ptr<Program> parse();

};


class Library
{
public:
    Library();
    explicit Library(std::unique_ptr<Loader> &&loader);
    virtual ~Library();

protected:
    unsigned int m_max_include_depth;
    std::unique_ptr<Loader> m_loader;
    std::unordered_map<std::string, std::unique_ptr<Program> > m_cache;

protected:
    void resolve_includes(Program *in_program, unsigned int depth);
    virtual const Program *_load(const std::string &path, unsigned int depth);

public:
    const Program *load(const std::string &path);

public:
    inline void set_loader(std::unique_ptr<Loader> &&loader)
    {
        m_loader = std::move(loader);
    }

    inline void set_max_include_depth(unsigned int depth)
    {
        m_max_include_depth = depth;
    }

};


class EvaluationContext
{
public:
    typedef std::tuple<std::string, std::string> Define;

public:
    explicit EvaluationContext(Library &library);
    EvaluationContext(const EvaluationContext &ref) = default;

private:
    Library &m_library;
    std::set<std::string> m_define_names;
    std::vector<Define> m_defines;

public:
    void define(const std::string &name, const std::string &rhs);
    void define1ull(const std::string &name, const unsigned long long value);
    void define1ll(const std::string &name, const signed long long value);
    void define1f(const std::string &name, const float value);
    void define1d(const std::string &name, const double value);

    inline const std::vector<Define> &defines() const
    {
        return m_defines;
    }

};


}

#endif
