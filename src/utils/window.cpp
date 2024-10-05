#include "window.hpp"

namespace sim::utils::window
{

class Window::Impl final
{
public:
    Impl(HWND handle) : handle_(handle) {}
    ~Impl() {}

    void Activate() const {
        // https://learn.microsoft.com/ja-jp/windows/win32/api/winuser/nf-winuser-setforegroundwindow
        SetForegroundWindow(handle_);
    }

private:
    HWND handle_;
};

Window::Window(HWND handle) noexcept
    : impl_(std::make_unique<Impl>(handle))
{
}

Window::~Window() = default;

Window::Window(Window&&) noexcept = default;
Window& Window::operator=(Window&&) noexcept = default;

void Window::Activate() const
{
    impl_->Activate();
}

}
