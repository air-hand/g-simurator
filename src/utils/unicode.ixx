export module utils:unicode;

import std;

namespace sim::utils::unicode
{
export std::wstring to_utf16(const std::string& utf8);
export std::string to_utf8(const std::wstring& utf16);
}
