module;

#include <dwmapi.h>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>

#include "logger.hpp"
#include "debug.hpp"

module utils;

import std;
import :logger;
import :capture;
import :time;
import :window_keyboard;

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

        if (GetForegroundWindow() == handle_) {
            DEBUG_LOG_ARGS("Already active {:p}, skip", (void*)handle_);
            return;
        }

        SetForegroundWindow(handle_);
        time::sleep(100); // Wait for activation to complete

        const auto foreground = GetForegroundWindow();
        [[maybe_unused]] const bool activated = (foreground == handle_);

        DEBUG_LOG_ARGS("Activation result - Target: {:p}, Foreground: {:p}, Success: {}",
            (void*)handle_, (void*)foreground, activated);

        DEBUG_ASSERT(activated);
    }

    std::string Name() const
    {
        DEBUG_LOG_SPAN(_);

        // Get the length of the window text first
        const int length = GetWindowTextLengthW(handle_);
        if (length == 0)
        {
            DEBUG_LOG_ARGS("Window has no title text OR GetWindowTextLengthW failed with error: {}", GetLastError());
            return "";
        }

        // Allocate buffer with length + 1 for null terminator
        std::vector<wchar_t> buffer(length + 1);
        const int copied = GetWindowTextW(handle_, buffer.data(), static_cast<int>(buffer.size()));
        if (copied == 0)
        {
            DEBUG_LOG_ARGS("GetWindowTextW failed with error: {}", GetLastError());
            return "";
        }

        return unicode::to_utf8(buffer.data());
    }

    WindowKeyboard Keyboard() const
    {
        return WindowKeyboard([this]{
            this->Activate();
        });
    }

    CaptureWindow CreateCapture() const
    {
        DEBUG_LOG_SPAN(_);
        return sim::utils::CaptureContext::Get().CaptureForWindowHandle(handle_);
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

WindowKeyboard Window::Keyboard() const
{
    return impl_->Keyboard();
}

std::string Window::Name() const
{
    return impl_->Name();
}

CaptureWindow Window::CreateCapture() const
{
    return impl_->CreateCapture();
}

}
