#include "windows.hpp"

#include "unicode.hpp"

namespace sim::utils::unicode
{

std::wstring to_utf16(const std::string& utf8)
{
    std::wstring utf16;
    if (utf8.empty())
    {
        return utf16;
    }
    int result = ::MultiByteToWideChar(
        CP_UTF8,
        MB_ERR_INVALID_CHARS,
        utf8.data(),
        utf8.length(),
        nullptr,
        0
    );
    if (result == 0)
    {
        return utf16;
    }
    std::wstring utf16(result, L'\0');
    ::MultiByteToWideChar(
        CP_UTF8,
        MB_ERR_INVALID_CHARS,
        utf8.data(),
        utf8.length(),
        utf16.data(),
        result
    );
    return utf16;
}

}
