module;

#include <cstdlib>

module utils;

import :env;

namespace sim::utils
{

std::optional<std::string> get_env(const char* name)
{
    std::size_t size = 0;
    if (getenv_s(&size, nullptr, 0, name) != 0 || size == 0)
    {
        return std::nullopt;
    }

    std::string value(size, '\0');
    if (getenv_s(&size, value.data(), value.size(), name) != 0 || size == 0)
    {
        return std::nullopt;
    }
    value.resize(size - 1); // remove trailing NUL
    return value;
}

}
