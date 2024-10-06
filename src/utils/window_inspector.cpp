#include "window_inspector.hpp"
#include "window.hpp"

#include "logger.hpp"

namespace sim::utils::window
{

class WindowInspector::Impl
{
public:
    Impl() {}
    ~Impl() {};

// EnumWindows for get target window handle.
// https://stackoverflow.com/a/51731567
    void EnumWindows() const
    {
        // https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-enumwindows
        // https://stackoverflow.com/a/51731567
        ::EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {
            wchar_t windowTitle[256] = {0};
            GetWindowTextW(hwnd, windowTitle, sizeof(windowTitle));
            
            logging::log(windowTitle);
            return TRUE;
        }, NULL);
    }

    std::unique_ptr<Window> Find(const char* windowName) const
    {
        // FIXME
        EnumWindows();
        // https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-enumwindows
        // https://stackoverflow.com/a/51731567
        HWND handle = FindWindowA(NULL, windowName);
        if (handle == NULL) {
            return nullptr;
        }
        return std::make_unique<Window>(handle);
    }
};

WindowInspector::WindowInspector() noexcept
    : impl_(std::make_unique<Impl>())
{
}

WindowInspector::~WindowInspector() = default;

WindowInspector::WindowInspector(WindowInspector&& rhs) noexcept : impl_(std::move(rhs.impl_))
{
}
WindowInspector& WindowInspector::operator=(WindowInspector&& rhs) noexcept
{
    if (this != &rhs)
    {
        impl_ = std::move(rhs.impl_);
    }
    return *this;
}

std::unique_ptr<Window> WindowInspector::Find(const char* windowName) const
{
    return std::move(impl_->Find(windowName));
}

}
