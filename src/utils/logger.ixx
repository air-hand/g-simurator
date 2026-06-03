module;

#include "macro.hpp"

export module utils:logger;

import std;
import :strings;

namespace sim::utils::logging
{

export void init();
export void log(std::string_view message);
export void log(std::wstring_view message);

template <typename ...Args> concept not_empty_args = (sizeof...(Args) > 0);

export template<
    typename... Args
>
requires not_empty_args<Args...>
void log(std::format_string<Args...> format, Args&&... args)
{
    log(std::format(format, std::forward<Args>(args)...));
}

export template<
    typename... Args
>
requires not_empty_args<Args...>
void log(std::wformat_string<Args...> format, Args&&... args)
{
    log(std::format(format, std::forward<Args>(args)...));
}

export class LogSpan final
{
public:
    explicit LogSpan(const std::string& message) noexcept;
    ~LogSpan();

    DELETE_COPY_AND_ASSIGN(LogSpan);
    DELETE_MOVE_AND_ASSIGN(LogSpan);
private:
    const std::string message_;
    const std::chrono::steady_clock::time_point start_;
};

}
