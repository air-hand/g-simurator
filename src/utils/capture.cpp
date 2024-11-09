module;

// https://blogs.windows.com/windowsdeveloper/2019/09/16/new-ways-to-do-screen-capture/
// https://tips.hecomi.com/entry/2021/03/23/230947

#include <directxtk/ScreenGrab.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Graphics.Capture.h>
#include <Windows.Graphics.Capture.Interop.h>
#include <winrt/Windows.Graphics.DirectX.h>
#include <winrt/Windows.Graphics.DirectX.Direct3d11.h>

#include "macro.hpp"
#include "logger.hpp"

// https://github.com/microsoft/DirectXTK/wiki/ScreenGrab
module utils;

import std;
import :logger;
import :capture;

// https://github.com/microsoft/cppwinrt/blob/b82340f3fce1ee9bbcc13151f6f74c374d03f24d/cppwinrt/main.cpp#L372

namespace sim::utils
{

CaptureContext::CaptureContext() noexcept = default;
CaptureContext::~CaptureContext() = default;

CaptureContext& CaptureContext::Get() noexcept
{
    static CaptureContext instance;
    return instance;
}

void CaptureContext::Init() /*const*/ noexcept
{
    DEBUG_LOG_SPAN(_);
    winrt::init_apartment();
    
    UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    D3D11CreateDevice(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        flags,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        device_.put(),
        nullptr,
        nullptr
    );
}

void CaptureContext::Finalize() const noexcept
{
    DEBUG_LOG_SPAN(_);
    winrt::uninit_apartment();
}

CaptureWindow CaptureContext::CaptureForWindowHandle(HWND handle) const
{
    return CaptureWindow(handle, device_);
}

//    // https://github.com/microsoft/Windows.UI.Composition-Win32-Samples/tree/master/cpp/ScreenCaptureforHWND
//    winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice device;
//    auto _device = winrt::Windows::Graphics::DirectX::Direct3D11::Get
//    winrt::Windows::Graphics::Capture::GraphicsCaptureItem item;
//    DirectX::SaveWICTextureToFile(device);

class CaptureWindow::Impl final
{
public:
    Impl(HWND hwnd, const DevicePtr& device) noexcept
        : hwnd_(hwnd), device_(device)
    {
    }

    DELETE_COPY_AND_ASSIGN(Impl);

    void Start() const
    {
        auto item = CreateCaptureItemForWindow();
    }
private:
    auto CreateCaptureItemForWindow() const
    {
        winrt::Windows::Graphics::Capture::GraphicsCaptureItem item = { nullptr };

        auto activation_factory = winrt::get_activation_factory<decltype(item)>();
        auto interop_factory = activation_factory.as<IGraphicsCaptureItemInterop>();
        interop_factory->CreateForWindow(hwnd_, winrt::guid_of<ABI::Windows::Graphics::Capture::IGraphicsCaptureItem>(), reinterpret_cast<void**>(winrt::put_abi(item)));
        return item;
    }

    const HWND hwnd_;
    const DevicePtr& device_;
};

CaptureWindow::CaptureWindow(HWND hwnd, const DevicePtr& device) noexcept
    : impl_(std::make_unique<Impl>(hwnd, device))
{
}
CaptureWindow::~CaptureWindow() = default;
CaptureWindow::CaptureWindow(CaptureWindow&&) noexcept = default;
CaptureWindow& CaptureWindow::operator=(CaptureWindow&&) noexcept = default;

void CaptureWindow::Start() const
{
    return impl_->Start();
}

}
