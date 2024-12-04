module;

#include <opencv2/opencv.hpp>
#include <opencv2/core/cuda.hpp>
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/cudaarithm.hpp>

#include <d3d11.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <Windows.Graphics.DirectX.Direct3D11.Interop.h>

#include <cuda_runtime.h>
#include <cuda_d3d11_interop.h>

#include "logger.hpp"

module utils;

import :image;
import :logger;

namespace sim::utils::image
{

//cv::cuda::GpuMat crop(const cv::cuda::GpuMat& input, const cv::Rect& roi)
//{
//    return input(roi);
//}
//
//cv::cuda::GpuMat grayScale(const cv::cuda::GpuMat& input)
//{
//    cv::cuda::GpuMat output;
//    cv::cuda::cvtColor(input, output, cv::COLOR_BGR2GRAY);
//    return output;
//}
//
//cv::cuda::GpuMat threshold(const cv::cuda::GpuMat& input)
//{
//    cv::cuda::GpuMat output;
//    cv::cuda::threshold(input, output, 128.0, 255.0, cv::THRESH_BINARY);
//    return output;
//}
//
//cv::Mat fromGPU(const cv::cuda::GpuMat& image)
//{
//    cv::Mat result;
//    image.download(result);
//    return result;
//}
//
//cv::cuda::GpuMat fromD3D11Texture2D([[maybe_unused]] const winrt::com_ptr<::ID3D11Texture2D>& texture)
//{
//    cv::cuda::GpuMat result;
//    assert(false);
//    return result;
//}

}
