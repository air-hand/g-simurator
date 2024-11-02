module;

#include "macro.hpp"

export module utils:logger;

import std;
import :strings;

namespace sim::utils::logging
{

export void init();
export void log(const std::string& message);
export void log(const std::wstring& message);

template <typename ...Args> concept not_empty_args = requires {
    sizeof...(Args) > 0;
};

export template<
    strings::char_type CharT,
    typename... Args
>
requires not_empty_args<Args...>
void log(const CharT* format, Args&&... args)
{
    constexpr std::size_t size = sizeof...(Args);
    static_assert(size > 0, "At least one argument is required");

    log(strings::fmt(format, std::forward<Args>(args)...));
}

export template<
    strings::char_type CharT
>
void log(const CharT* format)
{
    log(std::basic_string<CharT>(format));
}

export class LogSpan final
{
public:
    explicit LogSpan(const std::string& message) noexcept;
    ~LogSpan();
    LogSpan(LogSpan&&) = delete;
    LogSpan& operator=(LogSpan&&) = delete;

    DELETE_COPY_AND_ASSIGN(LogSpan);
private:
    const std::string message_;
};

}
