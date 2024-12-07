#include <gtest/gtest.h>

#include <opencv2/opencv.hpp>
#include <opencv2/core/cuda.hpp>
#include <opencv2/cudaarithm.hpp>
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/imgcodecs.hpp>

import utils;

namespace
{

namespace image = sim::utils::image;

TEST(test_image, clip)
{
    cv::cuda::GpuMat input(100, 100, CV_8UC4);
    cv::cuda::GpuMat foo = input(cv::Rect(10, 10, 20, 20));
    cv::cuda::GpuMat output;
    image::clip(input, cv::Rect(10, 10, 20, 20));
//    cv::cuda::cvtColor(input, output, cv::COLOR_BGR2GRAY);
}

}
