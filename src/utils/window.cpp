#include "windows.hpp"

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>

import utils.window;

namespace sim::utils::window
{

class Window::Impl final
{
public:
    explicit Impl(HWND handle) : handle_(handle) {}
    ~Impl() = default;

    void Activate() const
    {
        // https://learn.microsoft.com/ja-jp/windows/win32/api/winuser/nf-winuser-setforegroundwindow
        SetForegroundWindow(handle_);
    }

    void Capture() const
    {
        RECT rect;
        GetWindowRect(handle_, &rect);

        int width = rect.right - rect.left;
        int height = rect.bottom - rect.top;

        BITMAPINFO bmpInfo;
        bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmpInfo.bmiHeader.biWidth = rect.right;
        bmpInfo.bmiHeader.biHeight = -rect.bottom;
        bmpInfo.bmiHeader.biPlanes = 1;
        bmpInfo.bmiHeader.biBitCount = 24; //cv::Matの画像をアルファチャンネル有りにする場合は32;
        bmpInfo.bmiHeader.biCompression = BI_RGB;
      
        LPDWORD lpPixel;
        HDC hDC = GetDC(handle_);
        HBITMAP hBitmap = CreateDIBSection(hDC, &bmpInfo, DIB_RGB_COLORS, reinterpret_cast<void**>(&lpPixel), NULL, 0);
        HDC hMemDC = CreateCompatibleDC(hDC);
        SelectObject(hMemDC, hBitmap);
      
        cv::Mat monitor_img;
        monitor_img.create(height, width, CV_8UC3);

        BitBlt(hMemDC, rect.left, rect.top, width, height, hDC, 0, 0, SRCCOPY);
        GetDIBits(hMemDC, hBitmap, 0, height, monitor_img.data, reinterpret_cast<BITMAPINFO*>(&bmpInfo), DIB_RGB_COLORS);

        cv::imwrite("sample.jpeg", monitor_img);

        ReleaseDC(handle_, hDC);
        DeleteDC(hMemDC);
        DeleteObject(hBitmap);
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

void Window::Capture() const
{
    impl_->Capture();
}

}
