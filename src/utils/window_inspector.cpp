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

#if DEBUG
    void EnumWindows() const
    {
        // https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-enumwindows
        ::EnumWindows([](HWND hWnd, LPARAM lParam) -> BOOL {
            if (!IsWindowVisible(hWnd)) {
                return TRUE;
            }
            if (IsIconic(hWnd)) {
                return TRUE;
            }
            wchar_t windowTitle[256] = {NULL};
            GetWindowTextW(hWnd, windowTitle, sizeof(windowTitle));
            
            logging::log(windowTitle);
            return TRUE;
        }, NULL);
    }
#endif

    std::unique_ptr<Window> Find(const wchar_t* windowName) const
    {
        const auto handle = FindWindowW(NULL, windowName);
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

std::unique_ptr<Window> WindowInspector::Find(const wchar_t* windowName) const
{
    return std::move(impl_->Find(windowName));
}

}
