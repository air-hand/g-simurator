module;

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>

#include "logger.hpp"

module utils;

import std;
import :logger;
import :capture;
import :time;

namespace sim::utils::window
{

class Window::Impl final
{
public:
    explicit Impl(HWND handle) : handle_(handle) {}
    ~Impl() = default;

    void Activate() const
    {
        DEBUG_LOG_SPAN(_);
        // https://learn.microsoft.com/ja-jp/windows/win32/api/winuser/nf-winuser-setforegroundwindow
        SetForegroundWindow(handle_);
    }

    CaptureWindow CreateCapture() const 
    {
        DEBUG_LOG_SPAN(_);
        return sim::utils::CaptureContext::Get().CaptureForWindowHandle(handle_);
    }

    void Capture() const
    {
        DEBUG_LOG_SPAN(_);
        const auto c = sim::utils::CaptureContext::Get().CaptureForWindowHandle(handle_);
        c.Start();
    }

private:
    HWND handle_;
};

Window::Window(HWND handle) noexcept
    : impl_(std::make_unique<Impl>(handle))
{
}

Window::~Window() = default;

Window::Window(Window&& rhs) noexcept : impl_(std::move(rhs.impl_))
{
}
Window& Window::operator=(Window&& rhs) noexcept
{
    if (this != &rhs)
    {
        impl_ = std::move(rhs.impl_);
    }
    return *this;
}

void Window::Activate() const
{
    impl_->Activate();
}

CaptureWindow Window::CreateCapture() const
{
    return impl_->CreateCapture();
}

}
