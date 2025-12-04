module;

#include <opencv2/opencv.hpp>
#include <winrt/Windows.Graphics.DirectX.h>
#include <winrt/Windows.Graphics.DirectX.Direct3d11.h>
#include <Windows.Graphics.DirectX.Direct3D11.Interop.h>
#include <directxtk/ScreenGrab.h>

#include "macro.hpp"

export module utils:capture;

import std;

namespace sim::utils
{

template <typename T>
auto GetDXGIInterfaceFromObject(const winrt::Windows::Foundation::IInspectable& object)
{
    auto access = object.as<::Windows::Graphics::DirectX::Direct3D11::IDirect3DDxgiInterfaceAccess>();
    winrt::com_ptr<T> result;
    winrt::check_hresult(access->GetInterface(winrt::guid_of<T>(), result.put_void()));
    return result;
}

using Device = winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice;

export class CapturedImage final
{
public:
    explicit CapturedImage(const std::filesystem::path& path) noexcept;
    ~CapturedImage();

    DELETE_COPY_AND_ASSIGN(CapturedImage);

    CapturedImage(CapturedImage&&) noexcept;
    CapturedImage& operator=(CapturedImage&&) noexcept;

    cv::Mat Read() const;
    std::filesystem::path Path() const;
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

export class CaptureWindow final
{
public:
    CaptureWindow(HWND hwnd, const Device& device) noexcept;
    ~CaptureWindow();
    CaptureWindow(CaptureWindow&&) noexcept;
    CaptureWindow& operator=(CaptureWindow&&) noexcept;

    DELETE_COPY_AND_ASSIGN(CaptureWindow);

    void Start() const;
    CapturedImage Pop() const;
    std::optional<CapturedImage> TryPop(std::chrono::milliseconds timeout) const;
    void ClearBuffer() const;

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
    void Finalize() /*const*/ noexcept;

private:
    CaptureContext() noexcept;

    Device device_;
};

}
