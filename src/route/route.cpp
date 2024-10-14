#include <filesystem>

#include <google/protobuf/util/json_util.h>

#include "route.hpp"
#include "../utils/logger.hpp"
#include "../utils/file.hpp"

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
        std::string json;
        {
            auto json_file = utils::open_file(path, std::ios::in);
            json_file->seekg(0, std::ios::end);
            const auto size = json_file->tellg();
            json.resize(size, '\0');
            json_file->seekg(0, std::ios::beg);
            json_file->read(json.data(), size);
        }
        logging::log("Read JSON file: {}", json);
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
