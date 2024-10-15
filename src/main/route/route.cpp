#include <filesystem>

#include <google/protobuf/util/json_util.h>

#include "../../utils/logger.hpp"
#include "../../utils/file.hpp"

#include "route.hpp"

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
        const auto json = utils::read_all(utils::open_file(path, std::ios::in));
        logging::log("Read JSON file: [{}]", json);
        Route route;
        if (const auto result = google::protobuf::util::JsonStringToMessage(json, &route); !result.ok()) {
            logging::log("Failed to parse JSON file: {}", result.ToString());
        }
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
