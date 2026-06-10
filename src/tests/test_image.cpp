#include <gtest/gtest.h>

#include <opencv2/opencv.hpp>
#include <opencv2/core/cuda.hpp>

import utils;

namespace
{

namespace image = sim::utils::image;

TEST(test_image, crop_relative_rect)
{
    cv::Mat input(100, 200, CV_8UC1);
    input.setTo(cv::Scalar(0));
    input.at<std::uint8_t>(20, 50) = 123;

    const auto output = image::crop(input, image::RelativeRect{
        .x = 0.25,
        .y = 0.20,
        .width = 0.50,
        .height = 0.40,
    });

    EXPECT_EQ(output.cols, 100);
    EXPECT_EQ(output.rows, 40);
    EXPECT_EQ(output.at<std::uint8_t>(0, 0), 123);
}

TEST(test_image, crop_relative_rect_clamps_to_image_bounds)
{
    cv::Mat input(100, 200, CV_8UC1);
    input.setTo(cv::Scalar(0));
    input.at<std::uint8_t>(90, 180) = 234;

    const auto output = image::crop(input, image::RelativeRect{
        .x = 0.90,
        .y = 0.90,
        .width = 0.50,
        .height = 0.50,
    });

    EXPECT_EQ(output.cols, 20);
    EXPECT_EQ(output.rows, 10);
    EXPECT_EQ(output.at<std::uint8_t>(0, 0), 234);
}

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
