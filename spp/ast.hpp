#ifndef SPP_AST_H
#define SPP_AST_H

#include <memory>
#include <vector>

namespace spp {


std::string escape(const std::string &src);
std::tuple<bool, std::string> unescape(const std::string &src);


enum class ProgramType {
    GENERIC = 0,
    TESSELATION = 1,
    VERTEX = 2,
    GEOMETRY = 3,
    FRAGMENT = 4
};

template <typename internal_iterator, typename for_class>
class DereferencingIterator
{
public:
    typedef std::bidirectional_iterator_tag iterator_category;
    typedef typename internal_iterator::difference_type difference_type;
    typedef typename internal_iterator::value_type::element_type &value_type;
    typedef value_type reference;
    typedef typename internal_iterator::value_type::element_type *&pointer;

public:
    DereferencingIterator(internal_iterator curr):
        m_curr(curr)
    {

    }

private:
    internal_iterator m_curr;

public:
    inline bool operator==(const DereferencingIterator &other) const
    {
        return (m_curr == other.m_curr);
    }

    inline bool operator!=(const DereferencingIterator &other) const
    {
        return (m_curr != other.m_curr);
    }

    inline DereferencingIterator operator++(int) const
    {
        return DereferencingIterator(std::next(m_curr));
    }

    inline DereferencingIterator &operator++()
    {
        ++m_curr;
        return *this;
    }

    inline DereferencingIterator operator--(int) const
    {
        return DereferencingIterator(std::prev(m_curr));
    }

    inline DereferencingIterator &operator--()
    {
        --m_curr;
        return *this;
    }

    inline value_type operator*() const
    {
        return **m_curr;
    }

    inline value_type operator->() const
    {
        return **m_curr;
    }

    friend for_class;
};

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


class VersionDeclaration: public Section
{
public:
    VersionDeclaration(unsigned int version,
                       const std::string &profile,
                       ProgramType type);

private:
    unsigned int m_version;
    std::string m_profile;
    ProgramType m_type;

public:
    inline unsigned int version() const
    {
        return m_version;
    }

    inline const std::string &profile() const
    {
        return m_profile;
    }

    inline ProgramType type() const
    {
        return m_type;
    }

public:
    void evaluate(std::ostream &into) override;

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


class IncludeDirective: public Section
{
public:
    explicit IncludeDirective(const std::string &path);

private:
    std::string m_path;

public:
    void evaluate(std::ostream &into) override;

    inline std::string &path()
    {
        return m_path;
    }

    inline const std::string &path() const
    {
        return m_path;
    }

};


class Program
{
protected:
    typedef std::vector<std::unique_ptr<Section> > container_type;

public:
    typedef DereferencingIterator<typename container_type::iterator, Program> iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef typename container_type::size_type size_type;

public:
    Program();
    Program(const Program &src) = delete;
    Program(Program &&src) = delete;
    Program &operator=(const Program &src) = delete;
    Program &operator=(Program &&src) = delete;

private:
    ProgramType m_type;
    std::vector<std::unique_ptr<Section> > m_sections;

public:
    void append_section(std::unique_ptr<Section> &&sec);

    inline iterator begin()
    {
        return iterator(m_sections.begin());
    }

    inline iterator end()
    {
        return iterator(m_sections.end());
    }

    void evaluate(std::ostream &into);

    inline ProgramType type() const
    {
        return m_type;
    }

    void set_type(ProgramType new_type);

    inline size_type size() const
    {
        return m_sections.size();
    }


    inline Section &operator[](const size_type pos)
    {
        return *m_sections[pos];
    }

    inline const Section &operator[](const size_type pos) const
    {
        return *m_sections[pos];
    }
};

}

#endif
