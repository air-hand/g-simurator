#pragma once

#include <memory>

#include "macro.hpp"
#include "windows.hpp"

namespace sim::utils::window
{

class UTILS_EXPORT Window final
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
