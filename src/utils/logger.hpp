#pragma once

#include <string>

namespace sim::utils::logging
{

void init();
void log(const char* message);
void log(const std::string& message);
void log(const std::wstring& message);

template<
    typename T, typename... Args,
    typename std::enable_if_t<
        std::is_same_v<T, char> || std::is_same_v<T, wchar_t>
    >
>
void log(const std::basic_string<T>& format, Args&&... args)
{
    log(std::format(format, std::forward<Args>(args)...));
}

}
