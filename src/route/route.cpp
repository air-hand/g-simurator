#include <filesystem>
#include <fstream>

#include <google/protobuf/util/json_util.h>

#include "route.hpp"
#include "../utils/logger.hpp"

namespace sim::route
{

namespace logging = utils::logging;

class RouteReader::Impl
{
public:
    Impl() = default;
    ~Impl() = default;

    Route ReadJSONFile(const std::filesystem::path& path) const
    {
        std::ifstream json_file(path, std::ios::in);
        const auto json = std::string(std::istreambuf_iterator<char>(json_file), std::istreambuf_iterator<char>());
        logging::log("Read JSON file: {}", json);
        Route route;
        google::protobuf::util::JsonStringToMessage(json, &route);
        return route;
    }
};

RouteReader::RouteReader() noexcept
    : impl_(std::make_unique<Impl>())
{}

RouteReader::~RouteReader() = default;

RouteReader::RouteReader(RouteReader&&) noexcept = default;

RouteReader& RouteReader::operator=(RouteReader&& rhs)
{
    if (this != &rhs)
    {
        impl_ = std::move(rhs.impl_);
    }
    return *this;
}

Route RouteReader::ReadJSONFile(const std::filesystem::path& path) const
{
    return impl_->ReadJSONFile(path);
}

}
