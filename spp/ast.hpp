#ifndef SPP_AST_H
#define SPP_AST_H

#include <memory>
#include <vector>

#include "location.hh"

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


class EvaluationContext;


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
    explicit Section(const location &location);
    Section(const Section &ref) = delete;
    Section &operator=(const Section &ref) = delete;
    Section(Section &&src) = delete;
    Section &operator=(Section &&src) = delete;
    virtual ~Section();

protected:
    location m_location;

public:
    virtual std::unique_ptr<Section> copy() const = 0;
    virtual void evaluate(std::ostream &into, EvaluationContext &ctx) = 0;

public:
    inline const location &loc() const
    {
        return m_location;
    }

};


class VersionDeclaration: public Section
{
public:
    VersionDeclaration(const location &location,
                       unsigned int version,
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
    std::unique_ptr<Section> copy() const override;
    void evaluate(std::ostream &into, EvaluationContext &ctx) override;

};


class StaticSourceSection: public Section
{
public:
    StaticSourceSection() = default;
    explicit StaticSourceSection(const location &location,
                                 const std::string &source);

private:
    std::string m_source;

public:
    std::unique_ptr<Section> copy() const override;
    void evaluate(std::ostream &into, EvaluationContext &ctx) override;

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
    explicit IncludeDirective(const location &location, const std::string &path);

private:
    std::string m_path;

public:
    std::unique_ptr<Section> copy() const override;
    void evaluate(std::ostream &into, EvaluationContext &ctx) override;

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
    typedef DereferencingIterator<typename container_type::const_iterator, Program> const_iterator;
    typedef typename container_type::size_type size_type;

    typedef std::tuple<std::string, location, std::string> RecordedError;

public:
    explicit Program(const std::string &source_path = "<memory>");
    Program(const Program &src) = delete;
    Program(Program &&src) = delete;
    Program &operator=(const Program &src) = delete;
    Program &operator=(Program &&src) = delete;

private:
    ProgramType m_type;
    std::string m_source_path;
    std::vector<RecordedError> m_errors;
    std::vector<std::unique_ptr<Section> > m_sections;

public: // interface for the parser
    void add_local_error(const location &location,
                         const std::string &msg);
    void add_error(const RecordedError &ref);

    inline const std::vector<RecordedError> &errors() const
    {
        return m_errors;
    }

    void append_section(std::unique_ptr<Section> &&sec);

    inline ProgramType type() const
    {
        return m_type;
    }

    inline const std::string &source_path() const
    {
        return m_source_path;
    }

    void set_type(ProgramType new_type);


public: // container interface
    inline iterator begin()
    {
        return iterator(m_sections.begin());
    }

    inline const_iterator cbegin() const
    {
        return const_iterator(m_sections.cbegin());
    }

    inline iterator end()
    {
        return iterator(m_sections.end());
    }

    inline const_iterator cend() const
    {
        return const_iterator(m_sections.cend());
    }

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

    iterator erase(iterator iter);
    iterator erase(iterator first, iterator last);
    iterator insert(iterator before, std::unique_ptr<Section> &&section);


public:
    std::unique_ptr<Program> copy() const;

    void evaluate(std::ostream &into, EvaluationContext &ctx) const;
};

}

#endif
