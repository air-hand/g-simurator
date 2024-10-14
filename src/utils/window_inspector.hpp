#pragma once

#include <memory>

#include "macro.hpp"
#include "window.hpp" // use unique_ptr<Window> for complete type

namespace sim::utils::window
{

class UTILS_EXPORT WindowInspector final
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
