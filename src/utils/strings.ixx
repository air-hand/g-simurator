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
    constexpr bool all_valid = (... && (std::is_arithmetic_v<Args> || std::is_convertible_v<Ars, std::basic_string<CharT>));
    static_assert(all_valid, "fmt arguments must be number or string types.");

    constexpr const std::size_t size = sizeof...(Args);
    std::basic_string<CharT> buf;
    if constexpr (size == 0)
    {
        buf.assign(format);
        return buf;
    }
    else if constexpr (std::is_same_v<CharT, wchar_t>)
    {
        std::vformat_to(std::back_inserter(buf), format, std::make_wformat_args(args...));
    }
    else
    {
        std::vformat_to(std::back_inserter(buf), format, std::make_format_args(args...));
    }
    return buf;
}

}
