module;

#include <opencv2/opencv.hpp>
#include <opencv2/core/cuda.hpp>

#include "macro.hpp"

export module utils:image;

namespace sim::utils::image
{

export cv::cuda::GpuMat clip(const cv::cuda::GpuMat& input, const cv::Rect& roi);
export cv::cuda::GpuMat grayScale(const cv::cuda::GpuMat& input);
export cv::cuda::GpuMat threshold(const cv::cuda::GpuMat& input);

export cv::Mat fromGPU(const cv::cuda::GpuMat& input);

export void saveImage(const cv::Mat& image, const std::string& filename);

}
