#pragma once

#include <memory>

#include "macro.hpp"
#include "window.hpp" // use unique_ptr<Window> for complete type

namespace sim::utils::window
{

class WindowInspector final
{
public:
    UTILS_EXPORT WindowInspector() noexcept;
    UTILS_EXPORT ~WindowInspector();
    UTILS_EXPORT WindowInspector(WindowInspector&&) noexcept;
    UTILS_EXPORT WindowInspector& operator=(WindowInspector&&) noexcept;

    DELETE_COPY_AND_ASSIGN(WindowInspector);

    UTILS_EXPORT std::unique_ptr<Window> Find(const std::string& windowName) const;
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

}
