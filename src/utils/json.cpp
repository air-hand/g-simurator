module;

#include <nlohmann/json.hpp>

module utils;

namespace sim::utils::json
{

std::string remove_json_comments(const std::string& json_string)
{
    auto j = nlohmann::json::parse(json_string, nullptr, true, true);
    return j.dump();
}

}
