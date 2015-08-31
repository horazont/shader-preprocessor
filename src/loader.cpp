#include "spp/loader.hpp"

#include <fstream>

namespace spp {

/* spp::Loader */

Loader::~Loader()
{

}

/* spp::DefaultLoader */

std::unique_ptr<std::istream> DefaultLoader::open(const std::string &path)
{
    return std::make_unique<std::ifstream>(path);
}



}
