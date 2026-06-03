module;

#include <opencv2/opencv.hpp>
#include <opencv2/core/cuda.hpp>
#include <opencv2/cudaarithm.hpp>
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/cudawarping.hpp>
#include <opencv2/imgcodecs.hpp>

#include "debug.hpp"
#include "logger.hpp"

module utils;

import :image;
import :logger;

namespace sim::utils::image
{

cv::cuda::GpuMat grayScale(const cv::cuda::GpuMat& input)
{
    const auto channels = input.channels();
    DEBUG_LOG_ARGS("input channels: {}", channels);
    cv::cuda::GpuMat output;
    switch (channels)
    {
    case 1:
        output = input.clone();
        break;
    case 3:
        cv::cuda::cvtColor(input, output, cv::COLOR_BGR2GRAY);
        break;
    case 4:
        cv::cuda::cvtColor(input, output, cv::COLOR_BGRA2GRAY);
        break;
    default: // fallback
        DEBUG_ASSERT(false);
        output = input.clone();
    }
    return output;
}

cv::cuda::GpuMat threshold(const cv::cuda::GpuMat& input)
{
    cv::cuda::GpuMat output;
    cv::cuda::threshold(input, output, 128.0, 255.0, cv::THRESH_BINARY);
    return output;
}

cv::cuda::GpuMat resize(const cv::cuda::GpuMat& input, double scale_width, double scale_height)
{
    cv::cuda::GpuMat output;
    cv::cuda::resize(input, output, cv::Size(), scale_width, scale_height, cv::INTER_AREA);
    return output;
}

cv::Mat fromGPU(const cv::cuda::GpuMat& image)
{
    cv::Mat result;
    image.download(result);
    return result;
}

cv::Mat grayScale(const cv::Mat& input)
{
    const auto channels = input.channels();
    DEBUG_LOG_ARGS("input channels: {}", channels);
    cv::Mat output;
    switch (channels)
    {
    case 1:
        output = input.clone();
        break;
    case 3:
        cv::cvtColor(input, output, cv::COLOR_BGR2GRAY);
        break;
    case 4:
        cv::cvtColor(input, output, cv::COLOR_BGRA2GRAY);
        break;
    default:
        DEBUG_ASSERT(false);
        output = input.clone();
    }
    return output;
}

void saveImage(const cv::Mat& image, const std::string& filename)
{
    cv::imwrite(filename, image);
}

}
