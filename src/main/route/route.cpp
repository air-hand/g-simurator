#include "route.hpp"

#include <google/protobuf/util/json_util.h>

#include "utils/debug.hpp"
#include "utils/logger.hpp"

import std.compat;
import utils;

namespace sim::route
{

namespace logging = utils::logging;

class RouteReader::Impl
{
public:
    Impl() = default;
    ~Impl() = default;

    RouteList ReadJSONFile(const std::filesystem::path& path) const
    {
        DEBUG_LOG_SPAN(_);
        const auto json = utils::json::remove_json_comments(utils::read_all(utils::open_file(path, std::ios::in | std::ios::binary)));
        DEBUG_LOG_ARGS("Read JSON file: [{}]", json);
        RouteList route;
        if (const auto result = google::protobuf::util::JsonStringToMessage(json, &route); !result.ok())
        {
            logging::log("Failed to parse JSON file: {}", result.ToString());
            DEBUG_ASSERT(false);
        }
        if (!sim::utils::RouteValidator(route)())
        {
            logging::log("Invalid route file: {}", path.string());
            DEBUG_ASSERT(false);
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

RouteList RouteReader::ReadJSONFile(const std::filesystem::path& path) const
{
    return impl_->ReadJSONFile(path);
}

cv::Mat applyROI(const cv::Mat& input, const ROI& roi)
{
    DEBUG_LOG_SPAN(_);

    const auto width = input.cols;
    const auto height = input.rows;

    const auto roi_left = static_cast<int32_t>(std::floor(width * roi.left_percent() / 100));
    const auto roi_top = static_cast<int32_t>(std::floor(height * roi.top_percent() / 100));
    const auto roi_width = static_cast<int32_t>(std::floor(width * roi.width_percent() / 100));
    const auto roi_height = static_cast<int32_t>(std::floor(height * roi.height_percent() / 100));

    const auto rect = cv::Rect(roi_left, roi_top, roi_width, roi_height);
    DEBUG_LOG_ARGS("ROI: {} {} {} {}", rect.x, rect.y, rect.width, rect.height);
    return input(rect);
}

std::vector<WORD> keys(const route::Route& r)
{
    DEBUG_LOG_SPAN(_);

    const auto& keys = r.keys();
    std::vector<WORD> result;
    result.reserve(keys.size());

    for (const auto& key : keys)
    {
        int value = 0;
        if (key.starts_with("0x")) {
            const auto hex = key.substr(2);
            auto res = std::from_chars(hex.data(), hex.data() + hex.size(), value, 16);
            if (res.ec != std::errc{}) {
                logging::log("Failed to parse key '{}'", key);
                DEBUG_ASSERT(false);
                continue;
            }
        } else {
            value = static_cast<decltype(value)>(key.at(0));
        }
        if (value < 0x00 || 0xFE < value) {
            logging::log("Out of range '{:#X}'", value);
            DEBUG_ASSERT(false);
            continue;
        }
        result.emplace_back(static_cast<WORD>(value));
    }

    return result;
}

}
