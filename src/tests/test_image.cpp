#include <gtest/gtest.h>

#include <opencv2/opencv.hpp>
#include <opencv2/core/cuda.hpp>

import utils;

namespace
{

namespace image = sim::utils::image;

TEST(test_image, crop)
{
    if (!sim::utils::gpu::IsCudaAvailable())
    {
        GTEST_SKIP() << "CUDA is not available.";
    }
    cv::cuda::GpuMat input(100, 100, CV_8UC4);
    cv::cuda::GpuMat foo = input(cv::Rect(10, 10, 20, 20));
    cv::cuda::GpuMat output;
    image::crop(input, cv::Rect(10, 10, 20, 20));
//    cv::cuda::cvtColor(input, output, cv::COLOR_BGR2GRAY);
}

}
