#pragma once

#include <string>

#include "macro.hpp"

namespace sim::utils::unicode
{
UTILS_EXPORT std::wstring to_utf16(const std::string& utf8);
}
