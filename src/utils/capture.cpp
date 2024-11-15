module;

// https://blogs.windows.com/windowsdeveloper/2019/09/16/new-ways-to-do-screen-capture/
// https://tips.hecomi.com/entry/2021/03/23/230947

#include <wincodec.h>
#include <directxtk/ScreenGrab.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Graphics.Capture.h>
#include <Windows.Graphics.Capture.Interop.h>
#include <winrt/Windows.Graphics.DirectX.h>
#include <winrt/Windows.Graphics.DirectX.Direct3d11.h>
#include <Windows.Graphics.DirectX.Direct3D11.Interop.h>

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

CaptureContext::CaptureContext() noexcept : device_(nullptr) {}
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
#ifdef DEBUG
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    winrt::com_ptr<::ID3D11Device> d3d11Device;
    ::D3D11CreateDevice(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        flags,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        d3d11Device.put(),
        nullptr,
        nullptr
    );
    const auto dxgiDevice = d3d11Device.as<::IDXGIDevice>();
    winrt::com_ptr<::IInspectable> device;
    winrt::check_hresult(::CreateDirect3D11DeviceFromDXGIDevice(dxgiDevice.get(), device.put()));
    device_ = device.as<Device>();
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
    Impl(HWND hwnd, const Device& device) noexcept
        : hwnd_(hwnd), device_(device)
    {
    }

    DELETE_COPY_AND_ASSIGN(Impl);

    void Start() const
    {
        DEBUG_LOG_SPAN(_);
        auto item = CreateCaptureItemForWindow();
        const auto size = item.Size();
        DEBUG_LOG_ARGS("Capture window size: {}x{}", size.Width, size.Height);

        const auto d3dDevice = GetDXGIInterfaceFromObject<::ID3D11Device>(device_);
//        winrt::com_ptr<ID3D11DeviceContext> d3dContext{ nullptr };
        d3dDevice->GetImmediateContext(d3dContext_.put());

        swapChain_ = CreateDXGISwapChain(
            d3dDevice,
            static_cast<uint32_t>(size.Width),
            static_cast<uint32_t>(size.Height),
            static_cast<DXGI_FORMAT>(winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized),
            2
            );
        framePool_ = winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool::Create(
            device_,
            winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
            2,
            size
        );
        auto session_ = framePool_.CreateCaptureSession(item);
        // CaptureFramePoolのコンストラクタに合わせなければならない device, itemを受け取るこんすとらくた
        auto frameArrived_ = framePool_.FrameArrived(winrt::auto_revoke, { this, &Impl::OnFrame });

        session_.StartCapture();

        frameArrived_.revoke();
        framePool_.Close();
        session_.Close();
        swapChain_ = nullptr;
        framePool_ = nullptr;
        session_ = nullptr;
        item = nullptr;
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

    auto CreateDXGISwapChain(
        const winrt::com_ptr<ID3D11Device>& device,
        uint32_t width,
        uint32_t height,
        DXGI_FORMAT format,
        uint32_t bufferCount
    ) const
    {
        DXGI_SWAP_CHAIN_DESC1 desc = {};
        desc.Width = width;
        desc.Height = height;
        desc.Format = format;
        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.BufferCount = bufferCount;
        desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
        desc.Flags = 0;
        desc.Scaling = DXGI_SCALING_STRETCH;
        desc.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;

        const auto dxgiDevice = device.as<::IDXGIDevice2>();
        winrt::com_ptr<::IDXGIAdapter> adapter;
        winrt::check_hresult(dxgiDevice->GetParent(winrt::guid_of<::IDXGIAdapter>(), adapter.put_void()));
        winrt::com_ptr<::IDXGIFactory2> factory;
        winrt::check_hresult(adapter->GetParent(winrt::guid_of<::IDXGIFactory2>(), factory.put_void()));

        winrt::com_ptr<::IDXGISwapChain1> swapChain;
        winrt::check_hresult(factory->CreateSwapChainForComposition(
            device.get(),
            &desc,
            nullptr,
            swapChain.put()
        ));
        return swapChain;
    }

    void OnFrameArrived(
        const winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool& sender,
        [[maybe_unused]] const winrt::Windows::Foundation::IInspectable& args
    ) const
    {
        DEBUG_LOG_SPAN(_);
        {
            const auto frame = sender.TryGetNextFrame();
            const auto frameContentSize = frame.ContentSize();

            winrt::com_ptr<::ID3D11Texture2D> backBuffer;
            swapChain_->GetBuffer(0, winrt::guid_of<::ID3D11Texture2D>(), backBuffer.put_void());
            DirectX::SaveWICTextureToFile(d3dContext_.get(), backBuffer.get(), GUID_ContainerFormatPng, L"capture.png");
        }
        DXGI_PRESENT_PARAMETERS params = {0};
        swapChain_->Present1(1, 0, &params);
    }

    const HWND hwnd_;
    const Device& device_;
    winrt::com_ptr<::IDXGISwapChain1> swapChain_;
    winrt::com_ptr<::ID3D11DeviceContext> d3dContext_;
    winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool framePool_;
};

CaptureWindow::CaptureWindow(HWND hwnd, const Device& device) noexcept
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
