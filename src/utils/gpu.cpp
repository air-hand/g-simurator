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

import :gpu;
import :logger;

namespace
{

class D3D11Texture2DToCUDAFunctor final
{
public:
    D3D11Texture2DToCUDAFunctor() noexcept : cudaResource_(nullptr)
    {
        DEBUG_LOG_SPAN(_);
    }
    ~D3D11Texture2DToCUDAFunctor() noexcept
    {
        DEBUG_LOG_SPAN(_);
        if (cudaResource_)
        {
            // 7. リソースをアンマップ
            cudaGraphicsUnmapResources(1, &cudaResource_, 0);

            // 8. CUDAリソースの登録解除
            cudaGraphicsUnregisterResource(cudaResource_);
        }
        cudaResource_ = nullptr;
    }

    cv::cuda::GpuMat operator()(/*const */::ID3D11Texture2D* d3dTexture)
    {
        // FIXME
        int width = 0, height = 0;
        cv::cuda::GpuMat gpuMat(height, width, CV_8UC4);

        cudaGraphicsD3D11RegisterResource(&cudaResource_, d3dTexture, cudaGraphicsRegisterFlags::cudaGraphicsRegisterFlagsNone);
        cudaGraphicsMapResources(1, &cudaResource_, 0);

        // get cudaArray from cudaResource
        cudaArray* cudaArray;
        cudaGraphicsSubResourceGetMappedArray(&cudaArray, cudaResource_, 0, 0);

        // cudaArray to GpuMat
        cudaMemcpy2DFromArray(gpuMat.data, gpuMat.step, cudaArray, 0, 0, width * 4, height, cudaMemcpyDeviceToDevice);

        return gpuMat;
    }
private:
    ::cudaGraphicsResource* cudaResource_;
};

}

namespace sim::utils::gpu
{

cv::cuda::GpuMat d3D11Texture2DToCUDA(/*const*/::ID3D11Texture2D* texture)
{
    return D3D11Texture2DToCUDAFunctor()(texture);
}

}
