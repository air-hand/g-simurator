module;

#include <opencv2/opencv.hpp>
#include <opencv2/core/cuda.hpp>

#include <d3d11.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <Windows.Graphics.DirectX.Direct3D11.Interop.h>

#include "macro.hpp"

export module utils:image;

namespace sim::utils::image
{

cv::cuda::GpuMat fromD3D11Texture2D(const winrt::com_ptr<::ID3D11Texture2D>& texture);

cv::cuda::GpuMat crop(const cv::cuda::GpuMat& input, const cv::Rect& roi);
cv::cuda::GpuMat grayScale(const cv::cuda::GpuMat& input);
cv::cuda::GpuMat threshold(const cv::cuda::GpuMat& input);

cv::Mat fromGPU(const cv::cuda::GpuMat& input);

}
