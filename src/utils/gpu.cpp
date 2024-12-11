module;

#include <opencv2/opencv.hpp>
#include <opencv2/core/cuda.hpp>
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/cudaarithm.hpp>
//#include <opencv2/cudawarping.hpp>

#include <d3d11.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <Windows.Graphics.DirectX.Direct3D11.Interop.h>

#include <cuda_runtime.h>
#include <cuda_d3d11_interop.h>
#include <driver_types.h>

#include "logger.hpp"
#include "macro.hpp"

module utils;

import std.compat;

import :gpu;
import :logger;

namespace
{

class D3D11Texture2DToGpuMatFunctor final
{
public:
    explicit D3D11Texture2DToGpuMatFunctor(::ID3D11Texture2D* d3dTexture) noexcept : cudaResource_(nullptr), width_(0), height_(0)
    {
        DEBUG_LOG_SPAN(_);

        {
            ::D3D11_TEXTURE2D_DESC desc;
            d3dTexture->GetDesc(&desc);
            width_ = desc.Width;
            height_ = desc.Height;
        }

        {
            const auto error = cudaGraphicsD3D11RegisterResource(&cudaResource_, d3dTexture, cudaGraphicsRegisterFlags::cudaGraphicsRegisterFlagsNone);
            if (error != cudaError::cudaSuccess)
            {
                DEBUG_LOG_ARGS("cudaGraphicsD3D11RegisterResource failed: {}", cudaGetErrorString(error));
                cudaResource_ = nullptr;
                return;
            }
        }
        {
            const auto error = cudaGraphicsMapResources(1, &cudaResource_, 0);
            if (error != cudaError::cudaSuccess)
            {
                DEBUG_LOG_ARGS("cudaGraphicsMapResources failed: {}", cudaGetErrorString(error));
                // デストラクタに解放させたいので nullptr にはしない
                return;
            }
        }
    }

    ~D3D11Texture2DToGpuMatFunctor() noexcept
    {
        DEBUG_LOG_SPAN(_);
        if (cudaResource_)
        {
            cudaGraphicsUnmapResources(1, &cudaResource_, 0);

            cudaGraphicsUnregisterResource(cudaResource_);
        }
        cudaResource_ = nullptr;
    }

    cv::cuda::GpuMat operator()() const
    {
        DEBUG_LOG_SPAN(_);
        cv::cuda::GpuMat gpuMat(height_, width_, CV_8UC4);
        if (cudaResource_ == nullptr)
        {
            return gpuMat;
        }

        // get cudaArray from cudaResource
        cudaArray* cudaArray;
        cudaGraphicsSubResourceGetMappedArray(&cudaArray, cudaResource_, 0, 0);

        // cudaArray to GpuMat
        cudaMemcpy2DFromArray(gpuMat.data, gpuMat.step, cudaArray, 0, 0, width_ * 4, height_, cudaMemcpyDeviceToDevice);

        return gpuMat;
    }
private:
    ::cudaGraphicsResource* cudaResource_;
    uint32_t width_;
    uint32_t height_;
};

}

namespace sim::utils::gpu
{

cv::cuda::GpuMat d3D11Texture2DToGpuMat(::ID3D11Texture2D* texture)
{
    return D3D11Texture2DToGpuMatFunctor(texture)();
}

bool IsCudaAvailable()
{
    int device_count = 0;
    const auto error_id = cudaGetDeviceCount(&device_count);
    return error_id == cudaSuccess && device_count > 0;
}

}
