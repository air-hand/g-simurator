module;

//#include <d3d11.h>
#include <winrt/Windows.Graphics.DirectX.h>
#include <winrt/Windows.Graphics.DirectX.Direct3d11.h>
#include <directxtk/ScreenGrab.h>

#include "macro.hpp"

export module utils:capture;

import std;

namespace sim::utils
{

using DevicePtr = winrt::com_ptr<ID3D11Device>;

export class CaptureWindow final
{
public:
    CaptureWindow(HWND hwnd, const DevicePtr& device) noexcept;
    ~CaptureWindow();
    CaptureWindow(CaptureWindow&&) noexcept;
    CaptureWindow& operator=(CaptureWindow&&) noexcept;

    DELETE_COPY_AND_ASSIGN(CaptureWindow);

    void Start() const;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

export class CaptureContext final
{
public:
    ~CaptureContext();
    DELETE_COPY_AND_ASSIGN(CaptureContext);

    static CaptureContext& Get() noexcept;

    CaptureWindow CaptureForWindowHandle(HWND handle) const;

    void Init() /* const*/ noexcept;
    void Finalize() const noexcept;

private:
    CaptureContext() noexcept;

    DevicePtr device_;
};

}
