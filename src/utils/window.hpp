#pragma once

#include <memory>

#include "macro.hpp"
#include "windows.hpp"

namespace sim::utils::window
{

class Window final
{
public:
    Window(HWND handle) noexcept;
    ~Window();
    Window(Window&&) noexcept;
    Window& operator=(Window&&) noexcept;

    DELETE_COPY_AND_ASSIGN(Window);

    void Activate() const;
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

}
