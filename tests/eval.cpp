#include <catch.hpp>

#include <unordered_map>

#include "spp/spp.hpp"


class DummyDataLoader: public spp::Loader
{
private:
    std::unordered_map<std::string, std::string> m_files;

public:
    void add_source(const std::string &path, const std::string &source)
    {
        m_files[path] = source;
    }

    std::unique_ptr<std::istream> open(const std::string &path) override
    {
        auto iter = m_files.find(path);
        if (iter == m_files.end()) {
            return nullptr;
        }

        return std::make_unique<std::istringstream>(iter->second);
    }
};

using namespace spp;

TEST_CASE("Library/resolve_include_on_load")
{
    auto ddl = std::make_unique<DummyDataLoader>();
    ddl->add_source("other.glsl", "#version 330 core\n"
                                  "foo\n"
                                  "bar\n");
    ddl->add_source("one.glsl", "#version 330 core\n"
                                "{% include \"other.glsl\" %}");
    Library lib(std::move(ddl));

    const Program *prog = lib.load("one.glsl");
    REQUIRE(prog);
    CHECK(prog->errors().empty());
    REQUIRE(prog->size() >= 3);
    CHECK(prog->size() == 3);

    const StaticSourceSection *source(dynamic_cast<const StaticSourceSection*>(&(*prog)[1]));
    REQUIRE(source);
    CHECK(source->source() == "foo\n");

    source = dynamic_cast<const StaticSourceSection*>(&(*prog)[2]);
    REQUIRE(source);
    CHECK(source->source() == "bar\n");
}

TEST_CASE("Library/recursive_include_removes_include_and_adds_error")
{
    auto ddl = std::make_unique<DummyDataLoader>();
    ddl->add_source("one.glsl", "#version 330 core\n"
                                "foo\n{% include \"one.glsl\" %}");
    Library lib(std::move(ddl));

    const Program *prog = lib.load("one.glsl");
    REQUIRE(prog);
    CHECK_FALSE(prog->errors().empty());
    CHECK(prog->size() == 2);

    EvaluationContext ctx(lib);
    std::ostringstream out;
    prog->evaluate(out, ctx);

    std::string expected("#version 330 core\n"
                         "foo\n");

    CHECK(out.str() == expected);
}

TEST_CASE("EvaluationContext/reject_duplicate_defines")
{
    Library lib;
    EvaluationContext ctx(lib);

    ctx.define("FOO", "BAR");
    ctx.define("BAZ", "123");

    CHECK_THROWS_AS(ctx.define("FOO", "FNORD"), std::invalid_argument);
}

TEST_CASE("EvaluationContext/inject_defines")
{
    auto ddl = std::make_unique<DummyDataLoader>();
    ddl->add_source("one.glsl", "#version 330 core\n"
                                "foo\n");
    Library lib(std::move(ddl));

    const Program *prog = lib.load("one.glsl");
    REQUIRE(prog);
    CHECK(prog->errors().empty());
    CHECK(prog->size() == 2);

    EvaluationContext ctx(lib);

    ctx.define("FOO", "BAR");
    ctx.define1ull("BAZI", 123);
    ctx.define1d("BAZD", 1e-10);

    std::ostringstream out;
    prog->evaluate(out, ctx);

    std::string expected("#version 330 core\n"
                         "#define FOO BAR\n"
                         "#define BAZI 123\n"
                         "#define BAZD 1.00000000000000004e-10\n"
                         "foo\n");

    CHECK(out.str() == expected);
}
