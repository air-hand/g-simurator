module utils;

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
    utf16.resize(result);
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

std::string to_utf8(const std::wstring& utf16)
{
    if (utf16.empty())
    {
        return {};
    }
    int result = ::WideCharToMultiByte(
        CP_UTF8,
        WC_ERR_INVALID_CHARS,
        utf16.data(),
        utf16.length(),
        nullptr,
        0,
        NULL,
        NULL
    );
    if (result == 0)
    {
        return {};
    }
    std::string utf8(result, '\0');
    ::WideCharToMultiByte(
        CP_UTF8,
        WC_ERR_INVALID_CHARS,
        utf16.data(),
        utf16.length(),
        utf8.data(),
        result,
        NULL,
        NULL
    );
    return utf8;
}

}
