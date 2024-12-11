module;

#include <d3d11.h>
#include <opencv2/core/cuda.hpp>

export module utils:gpu;

namespace sim::utils::gpu
{
export cv::cuda::GpuMat d3D11Texture2DToGpuMat(::ID3D11Texture2D* texture);
export bool IsCudaAvailable();
}
