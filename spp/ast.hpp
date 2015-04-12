#ifndef SPP_AST_H
#define SPP_AST_H

#include <memory>
#include <vector>

namespace spp {

/**
 * Section of a shader program. A section can be of different types, see the
 * subclasses.
 *
 * @see StaticSourceSection
 */
class Section
{
public:
    Section();
    Section(const Section &ref) = delete;
    Section &operator=(const Section &ref) = delete;
    Section(Section &&src) = delete;
    Section &operator=(Section &&src) = delete;
    virtual ~Section();

public:
    virtual void evaluate(std::ostream &into) = 0;

};


class StaticSourceSection: public Section
{
public:
    StaticSourceSection() = default;
    explicit StaticSourceSection(const std::string &source);

private:
    std::string m_source;

public:
    void evaluate(std::ostream &into) override;

    inline std::string &source()
    {
        return m_source;
    }

    inline const std::string &source() const
    {
        return m_source;
    }

};


class Program
{
public:
    enum class Type {
        TESSELATION = 0,
        VERTEX = 1,
        GEOMETRY = 2,
        FRAGMENT = 3
    };

public:
    Program();
    Program(const Program &src) = delete;
    Program(Program &&src) = delete;
    Program &operator=(const Program &src) = delete;
    Program &operator=(Program &&src) = delete;

private:
    Type m_type;
    std::vector<std::unique_ptr<Section> > m_sections;

public:
    void append_section(Section *sec);
    void evaluate(std::ostream &into);

};

}

#endif
