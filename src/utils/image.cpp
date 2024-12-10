module;

#include <opencv2/opencv.hpp>
#include <opencv2/core/cuda.hpp>
#include <opencv2/cudaarithm.hpp>
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/imgcodecs.hpp>

#include "logger.hpp"

module utils;

import :image;
import :logger;

namespace sim::utils::image
{

cv::cuda::GpuMat grayScale(const cv::cuda::GpuMat& input)
{
    cv::cuda::GpuMat output;
    DEBUG_LOG_ARGS("input channels: {}", input.channels());
    cv::cuda::cvtColor(input, output, cv::COLOR_BGRA2GRAY);
    return output;
}

cv::cuda::GpuMat threshold(const cv::cuda::GpuMat& input)
{
    cv::cuda::GpuMat output;
    cv::cuda::threshold(input, output, 128.0, 255.0, cv::THRESH_BINARY);
    return output;
}

cv::Mat fromGPU(const cv::cuda::GpuMat& image)
{
    cv::Mat result;
    image.download(result);
    return result;
}

void saveImage(const cv::Mat& image, const std::string& filename)
{
    cv::imwrite(filename, image);
}

}
