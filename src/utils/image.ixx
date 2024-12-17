module;

#include <opencv2/opencv.hpp>
#include <opencv2/core/cuda.hpp>

#include "macro.hpp"

export module utils:image;

import std;

namespace sim::utils::image
{

export template<typename T>
T crop(const T& input, const cv::Rect& roi)
{
    return input(roi);
}

export cv::cuda::GpuMat grayScale(const cv::cuda::GpuMat& input);
export cv::cuda::GpuMat threshold(const cv::cuda::GpuMat& input);
export cv::cuda::GpuMat resize(const cv::cuda::GpuMat& input, double scale_width, double scale_height);

export cv::Mat fromGPU(const cv::cuda::GpuMat& input);

export void saveImage(const cv::Mat& image, const std::string& filename);

}
