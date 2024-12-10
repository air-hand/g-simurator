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

#include <opencv2/opencv.hpp>

#include "macro.hpp"
#include "logger.hpp"

// https://github.com/microsoft/DirectXTK/wiki/ScreenGrab
module utils;

import std;
import :capture;
import :container;
import :gpu;
import :image;
import :logger;
import :time;

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

void CaptureContext::Finalize() noexcept
{
    DEBUG_LOG_SPAN(_);
    device_ = nullptr;
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
        : hwnd_(hwnd), device_(device), buffer_(30)
    {
    }
    ~Impl()
    {
        if (frameArrived_)
        {
            frameArrived_.revoke();
        }
        if (framePool_)
        {
            framePool_.Close();
        }
        if (session_)
        {
            session_.Close();
        }
        swapChain_ = nullptr;
        framePool_ = nullptr;
        session_ = nullptr;
        item_ = nullptr;
    }

    DELETE_COPY_AND_ASSIGN(Impl);

    void Start()
    {
        DEBUG_LOG_SPAN(_);

        // FIXME: Recreate tmp directory.
        std::filesystem::remove_all("./tmp");
        std::filesystem::create_directory(L"./tmp");

        item_ = CreateCaptureItemForWindow();
        const auto size = item_.Size();
        DEBUG_LOG_ARGS("Capture window size: {}x{}", size.Width, size.Height);

        const auto d3dDevice = GetDXGIInterfaceFromObject<::ID3D11Device>(device_);
//        winrt::com_ptr<ID3D11DeviceContext> d3dContext{ nullptr };
        d3dDevice->GetImmediateContext(d3dContext_.put());

        swapChain_ = CreateDXGISwapChain(
            d3dDevice,
            static_cast<uint32_t>(size.Width),
            static_cast<uint32_t>(size.Height),
            static_cast<::DXGI_FORMAT>(winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized),
            2
            );
        framePool_ = winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool::CreateFreeThreaded(
            device_,
            winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
            2,
            size
        );
        session_ = framePool_.CreateCaptureSession(item_);
        frameArrived_ = framePool_.FrameArrived(winrt::auto_revoke, { this, &Impl::OnFrameArrived });

        session_.StartCapture();
    }

    cv::Mat Pop()
    {
        return buffer_.pop();
    }
private:
    auto CreateCaptureItemForWindow() const
    {
        winrt::Windows::Graphics::Capture::GraphicsCaptureItem item = { nullptr };

        auto activation_factory = winrt::get_activation_factory<decltype(item)>();
        auto interop_factory = activation_factory.as<::IGraphicsCaptureItemInterop>();
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
    ) /*const*/
    {
        DEBUG_LOG_SPAN(_);
        {
            const auto frame = sender.TryGetNextFrame();
            const auto frameContentSize = frame.ContentSize();

            const auto frameSurface = GetDXGIInterfaceFromObject<::ID3D11Texture2D>(frame.Surface());

            winrt::com_ptr<::ID3D11Texture2D> backBuffer;
            winrt::check_hresult(swapChain_->GetBuffer(0, winrt::guid_of<::ID3D11Texture2D>(), backBuffer.put_void()));
            d3dContext_->CopyResource(backBuffer.get(), frameSurface.get());

            cv::Mat out;
            {
                DEBUG_LOG_SPAN(mat_roi);
                const auto gpuMat = gpu::d3D11Texture2DToGpuMat(backBuffer.get());
//                const auto clipped = image::clip(gpuMat, cv::Rect(0, 0, frameContentSize.Width, frameContentSize.Height))a
                DEBUG_LOG("Gray scaling...");
                const auto gray = image::grayScale(gpuMat);
                DEBUG_LOG("Thresholding...");
                const auto thresholded = image::threshold(gray);
                DEBUG_LOG("Convert GpuMat to Mat...");
                out = image::fromGPU(thresholded);
            }
            const auto pushed = buffer_.push(out, [&out](const auto& container)
            {
                if (container.empty())
                {
                    DEBUG_LOG("Container is empty.");
                    return true;
                }
                const cv::Mat& last_pushed = container.back();
                cv::Mat diff;
                cv::absdiff(out, last_pushed, diff);
                const auto non_zero = cv::countNonZero(diff);
                DEBUG_LOG_ARGS("Non-zero pixel count: {}", non_zero);
                // TODO: 要調整
                return non_zero > 0;
            });

#ifdef DEBUG
            if (pushed)
            {
                const auto now = std::chrono::system_clock::now();
                const auto filename = sim::utils::strings::fmt(L"./tmp/capture_{:%Y%m%d%H%M%S}.png", std::chrono::time_point_cast<std::chrono::milliseconds>(now));
                image::saveImage(out, sim::utils::unicode::to_utf8(filename));
            }
#endif
        }
        DXGI_PRESENT_PARAMETERS params = {0};
        swapChain_->Present1(1, 0, &params);
        sim::utils::time::sleep(100); // TODO
    }

    const HWND hwnd_;
    const Device& device_;
    winrt::Windows::Graphics::Capture::GraphicsCaptureItem item_ { nullptr };
    winrt::com_ptr<::IDXGISwapChain1> swapChain_ { nullptr };
    winrt::com_ptr<::ID3D11DeviceContext> d3dContext_ { nullptr };
    winrt::Windows::Graphics::Capture::GraphicsCaptureSession session_ { nullptr };
    winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool framePool_ { nullptr };
    winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool::FrameArrived_revoker frameArrived_;
    container::RingBuffer<cv::Mat> buffer_;
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

cv::Mat CaptureWindow::Pop() const
{
    return impl_->Pop();
}

}
