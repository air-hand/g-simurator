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
            char windowTitle[256];
            GetWindowTextA(hwnd, windowTitle, sizeof(windowTitle));
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

WindowInspector::WindowInspector(WindowInspector&&) noexcept = default;
WindowInspector& WindowInspector::operator=(const WindowInspector&& rhs) noexcept
{
    return std::move(rhs);
}

std::unique_ptr<Window> WindowInspector::Find(const char* windowName) const
{
    return std::move(impl_->Find(windowName));
}

}
