module;

#include "macro.hpp"

export module utils:window_inspector;

import std;
import :window;

namespace sim::utils::window
{

export class WindowInspector final
{
public:
    WindowInspector() noexcept;
    ~WindowInspector();
    WindowInspector(WindowInspector&&) noexcept;
    WindowInspector& operator=(WindowInspector&&) noexcept;

    DELETE_COPY_AND_ASSIGN(WindowInspector);

    std::unique_ptr<Window> Find(const std::string& windowName) const;
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

}
