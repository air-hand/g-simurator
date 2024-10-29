module;

#include "std/windows.hpp"
#include "macro.hpp"

export module utils.window;

import std;

namespace sim::utils::window
{

export class Window final
{
public:
    explicit Window(HWND handle) noexcept;
    ~Window();
    Window(Window&&) noexcept;
    Window& operator=(Window&&) noexcept;

    DELETE_COPY_AND_ASSIGN(Window);

    void Activate() const;
    void Capture() const;
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

}
