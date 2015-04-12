#include "spp/spp.hpp"

#include <memory>
#include <fstream>

int main(int argc, char *argv[])
{
    if (argc == 1) {
        std::cout << "usage: " << argv[0] << " INFILE" << std::endl;
        return 1;
    }

    std::ifstream infile(argv[1]);
    spp::ParserContext ctx(infile);

    std::unique_ptr<spp::Program> prog = ctx.parse();
    prog->evaluate(std::cout);

    return 0;
}
