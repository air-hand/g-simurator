export module utils:strings;

import std;

namespace sim::utils::strings
{

export template<typename CharT> concept char_type = 
    std::is_same_v<CharT, char>
    || std::is_same_v<CharT, char8_t>
    || std::is_same_v<CharT, wchar_t>;

export template<
    char_type CharT,
    typename... Args
>
auto fmt(const CharT* format, Args&&... args)
{
    constexpr const std::size_t size = sizeof...(Args);
    const auto format_str = std::basic_string<CharT>(format);
    if constexpr (size == 0)
    {
        return format_str;
    }
    else if constexpr (std::is_same_v<CharT, wchar_t>)
    {
        return std::vformat(format_str, std::make_wformat_args(args...));
    }
    else
    {
        return std::vformat(format_str, std::make_format_args(args...));
    }
}

}
