#include "route.hpp"

#include <google/protobuf/util/json_util.h>

#include "utils/debug.hpp"
#include "utils/logger.hpp"

import std.compat;
import utils;

namespace
{
// https://learn.microsoft.com/ja-jp/windows/win32/inputdev/virtual-key-codes
std::optional<UINT> key_name_to_vk(const std::string& name)
{
    static const std::unordered_map<std::string, UINT> table{
        {"CTRL",   VK_CONTROL},
        {"SHIFT",  VK_SHIFT},
        {"ALT",    VK_MENU},
        {"LCTRL",  VK_LCONTROL},
        {"RCTRL",  VK_RCONTROL},
        {"LSHIFT", VK_LSHIFT},
        {"RSHIFT", VK_RSHIFT},
        {"LALT",   VK_LMENU},
        {"RALT",   VK_RMENU},

        {"ENTER",  VK_RETURN},
        {"ESC",    VK_ESCAPE},
        {"TAB",    VK_TAB},
        {"SPACE",  VK_SPACE},
        {"LEFT",   VK_LEFT},
        {"RIGHT",  VK_RIGHT},
        {"UP",     VK_UP},
        {"DOWN",   VK_DOWN},

        {"F1",  VK_F1},  {"F2",  VK_F2},  {"F3",  VK_F3},  {"F4",  VK_F4},
        {"F5",  VK_F5},  {"F6",  VK_F6},  {"F7",  VK_F7},  {"F8",  VK_F8},
        {"F9",  VK_F9},  {"F10", VK_F10}, {"F11", VK_F11}, {"F12", VK_F12},
    };

    auto it = table.find(name);
    if (it != table.end()) {
        return std::make_optional(it->second);
    }

    if (name.size() != 1) {
        DEBUG_ASSERT(false);
        return std::nullopt;
    }

        const auto ch = static_cast<unsigned char>(name.at(0));

        SHORT r = VkKeyScanA(ch);
        if (r == -1) {
            DEBUG_LOG_ARGS("VkKeyScanA failed for char: {}", name);
            DEBUG_ASSERT(false);
            return std::nullopt;
        }
        return std::make_optional(LOBYTE(r));
}
}

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
        const auto k = key_name_to_vk(key);
        if (k) {
            result.emplace_back(*k);
        }
    }

    return result;
}

}
