#pragma once

#include <memory>

#include "macro.hpp"
#include "windows.hpp"

namespace sim::utils::window
{

class Window final
{
public:
    UTILS_EXPORT explicit Window(HWND handle) noexcept;
    UTILS_EXPORT ~Window();
    UTILS_EXPORT Window(Window&&) noexcept;
    UTILS_EXPORT Window& operator=(Window&&) noexcept;

    DELETE_COPY_AND_ASSIGN(Window);

    UTILS_EXPORT void Activate() const;
    UTILS_EXPORT void Capture() const;
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

}
