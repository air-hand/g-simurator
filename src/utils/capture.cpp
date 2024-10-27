module;

#include <functional>

#include "windows.hpp"

// https://blogs.windows.com/windowsdeveloper/2019/09/16/new-ways-to-do-screen-capture/
// https://tips.hecomi.com/entry/2021/03/23/230947

#include <directxtk/ScreenGrab.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Graphics.Capture.h>
#include <winrt/Windows.Graphics.DirectX.h>
#include <winrt/Windows.Graphics.DirectX.Direct3d11.h>

// https://github.com/microsoft/DirectXTK/wiki/ScreenGrab
module utils.capture;

namespace sim::utils
{

Capture::Capture() noexcept = default;
Capture::~Capture() = default;

Capture& Capture::Get() noexcept
{
    static Capture instance;
    return instance;
}

void Capture::Init() const noexcept
{
    winrt::init_apartment();
}

void Capture::Finalize() const noexcept
{
    winrt::uninit_apartment();
}

//void capture()
//{
//    // https://github.com/microsoft/Windows.UI.Composition-Win32-Samples/tree/master/cpp/ScreenCaptureforHWND
//    winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice device;
//    auto _device = winrt::Windows::Graphics::DirectX::Direct3D11::Get
//    winrt::Windows::Graphics::Capture::GraphicsCaptureItem item;
//    DirectX::SaveWICTextureToFile(device);
//}

}
