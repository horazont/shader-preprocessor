#ifndef SPP_LOADER_H
#define SPP_LOADER_H

#include <istream>
#include <memory>
#include <string>


namespace spp {

class Loader
{
public:
    virtual ~Loader();

public:
    virtual std::unique_ptr<std::istream> open(const std::string &path) = 0;

};


class DefaultLoader: public Loader
{
public:
    std::unique_ptr<std::istream> open(const std::string &path) override;

};

}

#endif
