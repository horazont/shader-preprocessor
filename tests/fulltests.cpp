#include <catch.hpp>

#include <sstream>

#include "spp/spp.hpp"

#include "testdata.hpp"

using namespace spp;

TEST_CASE("fulltests/fragment_shader")
{
    std::istringstream data(test_glsl);
    ParserContext ctx(data);
    std::unique_ptr<Program> prog(ctx.parse());
    CHECK(ctx.errors().empty());
    REQUIRE(prog);
    CHECK(prog->type() == ProgramType::FRAGMENT);

    std::ostringstream out;
    prog->evaluate(out);
    CHECK(out.str() == std::string(test_glsl_processed));
}
