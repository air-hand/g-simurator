#include <functional>

#include "windows.hpp"
#include "capture.hpp"

// https://blogs.windows.com/windowsdeveloper/2019/09/16/new-ways-to-do-screen-capture/
// https://github.com/microsoft/Windows.UI.Composition-Win32-Samples/tree/master/cpp/ScreenCaptureforHWND
// https://tips.hecomi.com/entry/2021/03/23/230947

#include <directxtk/ScreenGrab.h>
#include <winrt/Windows.Foundation.Collections.h>
// https://github.com/microsoft/DirectXTK/wiki/ScreenGrab

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

}
