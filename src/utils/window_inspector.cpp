module utils;

import :unicode;
import :logger;

namespace sim::utils::window
{

class WindowInspector::Impl
{
public:
    Impl() {}
    ~Impl() {};

    void EnumWindows() const
    {
        // https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-enumwindows
        ::EnumWindows([](HWND hWnd, [[maybe_unused]] LPARAM lParam) noexcept -> BOOL
        {
            if (!IsWindowVisible(hWnd)) {
                return TRUE;
            }
            if (IsIconic(hWnd)) {
                return TRUE;
            }
            wchar_t windowTitle[256] = {NULL};
            GetWindowTextW(hWnd, windowTitle, std::size(windowTitle));
            
            logging::log(windowTitle);
            return TRUE;
        }, NULL);
    }

    std::unique_ptr<Window> Find(const std::string& windowName) const
    {
        const auto windowNameWide = unicode::to_utf16(windowName);
        const auto handle = FindWindowW(NULL, windowNameWide.c_str());
        if (handle == NULL)
        {
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

std::unique_ptr<Window> WindowInspector::Find(const std::string& windowName) const
{
    return impl_->Find(windowName);
}

}
