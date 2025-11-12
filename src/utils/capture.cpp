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

class CapturedImage::Impl final
{
public:
    Impl(const std::filesystem::path& path) noexcept : path_(path) {}
    ~Impl() = default;

    DELETE_COPY_AND_ASSIGN(Impl);

    cv::Mat Read() const
    {
        DEBUG_LOG_SPAN(_);
        return cv::imread(path_.string());
    }

    std::filesystem::path Path() const
    {
        return path_;
    }
private:
    const std::filesystem::path path_;
};

CapturedImage::CapturedImage(const std::filesystem::path& path) noexcept
    : impl_(std::make_unique<Impl>(path))
{
}
CapturedImage::~CapturedImage() = default;
CapturedImage::CapturedImage(CapturedImage&&) noexcept = default;
CapturedImage& CapturedImage::operator=(CapturedImage&&) noexcept = default;

cv::Mat CapturedImage::Read() const
{
    return impl_->Read();
}

std::filesystem::path CapturedImage::Path() const
{
    return impl_->Path();
}

CaptureContext::CaptureContext() noexcept : device_(nullptr) {}
CaptureContext::~CaptureContext() = default;

CaptureContext& CaptureContext::Get() noexcept
{
    static CaptureContext instance;
    return instance;
}

void CaptureContext::Init() noexcept
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

class CaptureWindow::Impl final
{
public:
    Impl(HWND hwnd, const Device& device) noexcept
        : hwnd_(hwnd), device_(device), buffer_(10000), out_dir_("./tmp"), callback_running_count_(0)
    {
    }
    ~Impl()
    {
        if (frameArrived_)
        {
            frameArrived_.revoke();
        }

        // revoke()は新しいコールバックを停止するが、実行中のコールバックは止まらない
        // 実行中のコールバックがImplのメンバ変数にアクセス中にImplが破棄されるとuse-after-freeになるため、
        // 全てのコールバックが終了するまで待機する
        while (callback_running_count_ > 0)
        {
            DEBUG_LOG_ARGS("Waiting for capture thread to finish... (remaining: {})", callback_running_count_.load());
            time::sleep(10);
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

        std::filesystem::remove_all(out_dir_);
        std::filesystem::create_directory(out_dir_);

        item_ = CreateCaptureItemForWindow();
        DEBUG_LOG_ARGS("Display Name: {}", sim::utils::unicode::to_utf8(item_.DisplayName().c_str()));
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

    CapturedImage Pop()
    {
        return buffer_.pop();
    }

    std::optional<CapturedImage> TryPop(std::chrono::milliseconds timeout)
    {
        return buffer_.try_pop(timeout);
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
        ++callback_running_count_;
        auto guard = std::unique_ptr<void, std::function<void(void*)>>(
            (void*)1, [this](void*) { --callback_running_count_; });

        DEBUG_LOG_SPAN(_);

        // FIXME: テスト用sleep（use-after-free再現用）
        DEBUG_LOG("Sleeping 40 seconds in callback...");
        time::sleep(40000);
        {
            // FIXME: Fix when changed window size.
            const auto frame = sender.TryGetNextFrame();
            const auto frameContentSize = frame.ContentSize();

            const auto frameSurface = GetDXGIInterfaceFromObject<::ID3D11Texture2D>(frame.Surface());

            winrt::com_ptr<::ID3D11Texture2D> backBuffer;
            winrt::check_hresult(swapChain_->GetBuffer(0, winrt::guid_of<::ID3D11Texture2D>(), backBuffer.put_void()));
            d3dContext_->CopyResource(backBuffer.get(), frameSurface.get());

            cv::Mat out;
            const auto now = std::chrono::system_clock::now();
            const auto output_to = out_dir_ / sim::utils::strings::fmt("{:%Y%m%d%H%M%S}", std::chrono::time_point_cast<std::chrono::milliseconds>(now));
            std::filesystem::create_directory(output_to);

            {
                DEBUG_LOG_SPAN(__);
                const auto gpuMat = gpu::d3D11Texture2DToGpuMat(backBuffer.get());

                // OCR高速化のための処理パイプライン
                const std::vector<std::pair<std::string, std::function<cv::cuda::GpuMat(const cv::cuda::GpuMat&)>>> transforms = {
                    {"grayscale", &image::grayScale},
                    {"resize", [](const cv::cuda::GpuMat& img) {
                        DEBUG_LOG_ARGS("Image size: {}x{}", img.cols, img.rows);
                        if (img.cols >= 1280) {
                            DEBUG_LOG_ARGS("Resizing to 0.5x for faster OCR");
                            return image::resize(img, 0.5, 0.5);
                        }
                        return img;
                    }},
                    // {"threshold", &image::threshold},  // 二値化を無効化（Tesseractに任せる）
                };

                cv::cuda::GpuMat processed = gpuMat;
                for (size_t i = 0; i < transforms.size(); ++i) {
                    const auto& [name, transform] = transforms[i];
#ifdef DEBUG
                    const auto filename = (output_to / sim::utils::strings::fmt("{}_{}_before.png", i, name)).string();
                    image::saveImage(image::fromGPU(processed), filename);
#endif
                    processed = transform(processed);
                }
                out = image::fromGPU(processed);
                DEBUG_LOG_ARGS("Final processed image size: {}x{}", out.cols, out.rows);
            }

            const auto filename = (output_to / "capture_target.png").string();
            image::saveImage(out, filename);

            buffer_.push(CapturedImage(filename));
        }
        DXGI_PRESENT_PARAMETERS params = {0};
        swapChain_->Present1(1, 0, &params);
    }

    const HWND hwnd_;
    const Device& device_;
    winrt::Windows::Graphics::Capture::GraphicsCaptureItem item_ { nullptr };
    winrt::com_ptr<::IDXGISwapChain1> swapChain_ { nullptr };
    winrt::com_ptr<::ID3D11DeviceContext> d3dContext_ { nullptr };
    winrt::Windows::Graphics::Capture::GraphicsCaptureSession session_ { nullptr };
    winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool framePool_ { nullptr };
    winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool::FrameArrived_revoker frameArrived_;
    container::RingBuffer<CapturedImage> buffer_;
    const std::filesystem::path out_dir_;
    std::atomic<int> callback_running_count_;
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

CapturedImage CaptureWindow::Pop() const
{
    return impl_->Pop();
}

std::optional<CapturedImage> CaptureWindow::TryPop(std::chrono::milliseconds timeout) const
{
    return impl_->TryPop(timeout);
}

}
