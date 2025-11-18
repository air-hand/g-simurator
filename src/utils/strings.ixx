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

export std::string remove_all_whitespaces(const std::string in)
{
    auto view = in | std::views::filter([](char c) {
        const unsigned char uc = static_cast<unsigned char>(c);
        // skip if ASCII space
        if (uc < 0x80 && (uc == ' ' || uc == '\t')) {
            return false;
        }
        return true;
    });
    return std::string(view.begin(), view.end());
}


}
