#pragma once

#include <memory>

#include "macro.hpp"
#include "window.hpp"

namespace sim::utils::window
{

class WindowInspector final
{
public:
    WindowInspector() noexcept;
    ~WindowInspector();
    WindowInspector(WindowInspector&&) noexcept;
    WindowInspector& operator=(const WindowInspector&&) noexcept;

    DELETE_COPY_AND_ASSIGN(WindowInspector);

    std::unique_ptr<Window> Find(const char* windowName) const;
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

}
