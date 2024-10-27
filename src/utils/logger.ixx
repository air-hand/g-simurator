export module utils.logger;

import std;

namespace sim::utils::logging
{

export void init();
export void log(const std::string& message);
export void log(const std::wstring& message);

template <typename ...Args> concept not_empty_args = requires {
    sizeof...(Args) > 0;
};

template<typename CharT> concept char_type = 
    std::is_same_v<CharT, char>
    || std::is_same_v<CharT, char8_t>
    || std::is_same_v<CharT, wchar_t>;

export template<
    char_type CharT,
    typename... Args
>
requires not_empty_args<Args...>
void log(const CharT* format, Args&&... args)
{
    constexpr std::size_t size = sizeof...(Args);
    static_assert(size > 0, "At least one argument is required");

    const auto fmt = std::basic_string_view<CharT>(format);
    if constexpr (std::is_same_v<CharT, wchar_t>)
    {
        log(std::vformat(fmt, std::make_wformat_args(args...)));
    }
    else
    {
        log(std::vformat(fmt, std::make_format_args(args...)));
    }
}

export template<
    char_type CharT
>
void log(const CharT* format)
{
    log(std::basic_string<CharT>(format));
}

}
