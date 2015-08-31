#include <catch.hpp>

#include <sstream>

#include "spp/context.hpp"


using namespace spp;


template <typename InputIterator>
void dump_errors(InputIterator begin, const InputIterator &end)
{
    for (InputIterator i = begin; i != end; ++i)
    {
        std::cout << "error during parsing: " << std::get<1>(*i) << std::endl;
    }
}


TEST_CASE("parser/version_directive/fragment")
{
    std::istringstream data("#version 330 core fragment\n"
                            "");

    ParserContext ctx(data);
    std::unique_ptr<Program> prog(ctx.parse());
    CHECK(ctx.errors().empty());
    dump_errors(ctx.errors().begin(), ctx.errors().end());
    REQUIRE(prog);

    REQUIRE(prog->size() >= 1);
    CHECK(prog->size() == 1);

    VersionDeclaration *version = dynamic_cast<VersionDeclaration*>(&(*prog)[0]);
    REQUIRE(version);
    CHECK(version->type() == ProgramType::FRAGMENT);
    CHECK(version->version() == 330);
    CHECK(version->profile() == "core");
}

TEST_CASE("parser/version_directive/vertex")
{
    std::istringstream data("#version 330 core vertex\n"
                            "");

    ParserContext ctx(data);
    std::unique_ptr<Program> prog(ctx.parse());
    CHECK(ctx.errors().empty());
    dump_errors(ctx.errors().begin(), ctx.errors().end());
    REQUIRE(prog);

    REQUIRE(prog->size() >= 1);
    CHECK(prog->size() == 1);

    VersionDeclaration *version = dynamic_cast<VersionDeclaration*>(&(*prog)[0]);
    REQUIRE(version);
    CHECK(version->type() == ProgramType::VERTEX);
    CHECK(version->version() == 330);
    CHECK(version->profile() == "core");
}

TEST_CASE("parser/version_directive/without_type")
{
    std::istringstream data("#version 330 core\n"
                            "");

    ParserContext ctx(data);
    std::unique_ptr<Program> prog(ctx.parse());
    CHECK(ctx.errors().empty());
    dump_errors(ctx.errors().begin(), ctx.errors().end());
    REQUIRE(prog);

    REQUIRE(prog->size() >= 1);
    CHECK(prog->size() == 1);

    VersionDeclaration *version = dynamic_cast<VersionDeclaration*>(&(*prog)[0]);
    REQUIRE(version);
    CHECK(version->type() == ProgramType::GENERIC);
    CHECK(version->version() == 330);
    CHECK(version->profile() == "core");
}

TEST_CASE("parser/version_directive/without_trailing_newline")
{
    std::istringstream data("#version 330 core vertex");

    ParserContext ctx(data);
    std::unique_ptr<Program> prog(ctx.parse());
    CHECK(ctx.errors().empty());
    dump_errors(ctx.errors().begin(), ctx.errors().end());
    REQUIRE(prog);

    REQUIRE(prog->size() >= 1);
    CHECK(prog->size() == 1);

    VersionDeclaration *version = dynamic_cast<VersionDeclaration*>(&(*prog)[0]);
    REQUIRE(version);
    CHECK(version->type() == ProgramType::VERTEX);
    CHECK(version->version() == 330);
    CHECK(version->profile() == "core");
}


TEST_CASE("parser/version_directive/tesselation")
{
    std::istringstream data("#version 330 core tesselation\n"
                            "");

    ParserContext ctx(data);
    std::unique_ptr<Program> prog(ctx.parse());
    CHECK(ctx.errors().empty());
    dump_errors(ctx.errors().begin(), ctx.errors().end());
    REQUIRE(prog);

    REQUIRE(prog->size() >= 1);
    CHECK(prog->size() == 1);

    VersionDeclaration *version = dynamic_cast<VersionDeclaration*>(&(*prog)[0]);
    REQUIRE(version);
    CHECK(version->type() == ProgramType::TESSELATION);
    CHECK(version->version() == 330);
    CHECK(version->profile() == "core");
}

TEST_CASE("parser/version_directive/geometry")
{
    std::istringstream data("#version 330 core geometry\n"
                            "");

    ParserContext ctx(data);
    std::unique_ptr<Program> prog(ctx.parse());
    CHECK(ctx.errors().empty());
    dump_errors(ctx.errors().begin(), ctx.errors().end());
    REQUIRE(prog);

    REQUIRE(prog->size() >= 1);
    CHECK(prog->size() == 1);

    VersionDeclaration *version = dynamic_cast<VersionDeclaration*>(&(*prog)[0]);
    REQUIRE(version);
    CHECK(version->type() == ProgramType::GEOMETRY);
    CHECK(version->version() == 330);
    CHECK(version->profile() == "core");
}

TEST_CASE("parser/include_directive")
{
    std::istringstream data("#version 330 core fragment\n"
                            "{% include \"some \\\"string\\\" with \\n magic\" %}\n"
                            "");

    ParserContext ctx(data);
    std::unique_ptr<Program> prog(ctx.parse());
    CHECK(ctx.errors().empty());
    dump_errors(ctx.errors().begin(), ctx.errors().end());
    REQUIRE(prog);

    REQUIRE(prog->size() >= 2);
    CHECK(prog->size() == 3);

    IncludeDirective *include = dynamic_cast<IncludeDirective*>(&(*prog)[1]);
    REQUIRE(include);
    CHECK(include->path() == std::string("some \"string\" with \n magic"));
}

TEST_CASE("parser/include_directive/incorrect_escape_sequence")
{
    std::istringstream data("#version 330 core fragment\n"
                            "{% include \"\\x\" %}\n"
                            "{% include \"\\y\" %}\n"
                            "foo bar baz");

    ParserContext ctx(data);
    std::unique_ptr<Program> prog(ctx.parse());
    CHECK_FALSE(ctx.errors().empty());
    dump_errors(ctx.errors().begin(), ctx.errors().end());
    REQUIRE(prog);
    CHECK(prog->size() == 4);

    std::string expected("#version 330 core\n"
                         "\n\nfoo bar baz");
    std::ostringstream evaluated;
    prog->evaluate(evaluated);
    CHECK(evaluated.str() == expected);
}

TEST_CASE("parser/include_directive/without_terminating_newline")
{
    std::istringstream data("#version 330 core fragment\n"
                            "{% include \"foobar\" %}");

    ParserContext ctx(data);
    std::unique_ptr<Program> prog(ctx.parse());
    CHECK(ctx.errors().empty());
    dump_errors(ctx.errors().begin(), ctx.errors().end());
    REQUIRE(prog);

    REQUIRE(prog->size() >= 2);
    CHECK(prog->size() == 2);

    IncludeDirective *include = dynamic_cast<IncludeDirective*>(&(*prog)[1]);
    REQUIRE(include);
    CHECK(include->path() == std::string("foobar"));
}

TEST_CASE("parser/include_directive/evaluate_properly_escaped")
{
    IncludeDirective test("foo \"bar\" baz \n \r");
    std::ostringstream dest;
    test.evaluate(dest);
    CHECK(dest.str() == std::string("#include \"foo \\\"bar\\\" baz \\n \\r\"\n"));
}

TEST_CASE("parser/sourcecode")
{
    std::istringstream data("#version 330 core fragment\n"
                            "foo bar\n"
                            "baz");

    ParserContext ctx(data);
    std::unique_ptr<Program> prog(ctx.parse());
    CHECK(ctx.errors().empty());
    REQUIRE(prog);
    CHECK(prog->size() == 3);

    std::string expected("#version 330 core\n"
                         "foo bar\n"
                         "baz");
    std::ostringstream evaluated;
    prog->evaluate(evaluated);
    CHECK(evaluated.str() == expected);
}

TEST_CASE("parser/sourcecode/string_literals")
{
    std::istringstream data("#version 330 core fragment\n"
                            "foo bar \"foo \\\" bar \" baz");

    ParserContext ctx(data);
    std::unique_ptr<Program> prog(ctx.parse());
    CHECK(ctx.errors().empty());
    REQUIRE(prog);
    REQUIRE(prog->size() >= 2);

    std::string expected("#version 330 core\n"
                         "foo bar \"foo \\\" bar \" baz");
    std::ostringstream evaluated;
    prog->evaluate(evaluated);
    CHECK(evaluated.str() == expected);
}

TEST_CASE("parser/sourcecode/c++_style_comments")
{
    std::istringstream data("#version 330 core fragment\n"
                            "foo // baz \"foofoo\"\n"
                            "fnord");

    ParserContext ctx(data);
    std::unique_ptr<Program> prog(ctx.parse());
    CHECK(ctx.errors().empty());
    REQUIRE(prog);
    CHECK(prog->size() == 3);

    std::string expected("#version 330 core\n"
                         "foo // baz \"foofoo\"\n"
                         "fnord");
    std::ostringstream evaluated;
    prog->evaluate(evaluated);
    CHECK(evaluated.str() == expected);
}

TEST_CASE("parser/sourcecode/c_style_comments")
{
    std::istringstream data("#version 330 core fragment\n"
                            "foo /* baz \"foofoo\"\n"
                            "fnord */end");

    ParserContext ctx(data);
    std::unique_ptr<Program> prog(ctx.parse());
    CHECK(ctx.errors().empty());
    REQUIRE(prog);
    CHECK(prog->size() == 3);

    std::string expected("#version 330 core\n"
                         "foo /* baz \"foofoo\"\n"
                         "fnord */end");
    std::ostringstream evaluated;
    prog->evaluate(evaluated);
    CHECK(evaluated.str() == expected);
}
