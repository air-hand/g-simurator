module;

#include <d3d11.h>
#include <opencv2/core/cuda.hpp>

#include "macro.hpp"

export module utils:gpu;

import std;

namespace sim::utils::gpu
{
export cv::cuda::GpuMat d3D11Texture2DToCUDA(::ID3D11Texture2D* texture);
}
