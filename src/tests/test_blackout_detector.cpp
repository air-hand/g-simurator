#include <gtest/gtest.h>
#include <opencv2/opencv.hpp>

import std;
import utils;

namespace
{

namespace utils = sim::utils;
namespace training = utils::game::training;

TEST(test_blackout_detector, ok)
{
    const auto detector = training::BlackoutDetector(training::BlackoutDetectorConfig{
        .roi = cv::Rect(0, 0, 100, 100)
    });

    cv::Mat black(100, 100, CV_8UC3, cv::Scalar(0, 0, 0));

    EXPECT_TRUE(detector.IsBlackout(black));
}

TEST(test_blackout_detector, ok_roi)
{
    const auto detector = training::BlackoutDetector(training::BlackoutDetectorConfig{
        .roi = cv::Rect(10, 10, 30, 30)
    });

    cv::Mat image(100, 100, CV_8UC3, cv::Scalar(255, 255, 255));
    image(cv::Rect(10, 10, 30, 30)).setTo(cv::Scalar(0, 0, 0));

    EXPECT_TRUE(detector.IsBlackout(image));
}

TEST(test_blackout_detector, ng)
{
    const auto detector = training::BlackoutDetector(training::BlackoutDetectorConfig{
        .roi = cv::Rect(0, 0, 100, 100)
    });

    cv::Mat white(100, 100, CV_8UC3, cv::Scalar(255, 255, 255));

    EXPECT_FALSE(detector.IsBlackout(white));
}

TEST(test_blackout_detector, ng_roi)
{
    const auto detector = training::BlackoutDetector(training::BlackoutDetectorConfig{
        .roi = cv::Rect(10, 10, 30, 30)
    });

    cv::Mat image(100, 100, CV_8UC3, cv::Scalar(0, 0, 0));
    image(cv::Rect(20, 20, 10, 10)).setTo(cv::Scalar(255, 255, 255));

    EXPECT_FALSE(detector.IsBlackout(image));
}

TEST(test_blackout_detector, ok_with_small_bright_area)
{
    const auto detector = training::BlackoutDetector(training::BlackoutDetectorConfig{
        .roi = cv::Rect(10, 10, 40, 40),
        .maxBrightPixelRatio = 0.01
    });

    cv::Mat image(100, 100, CV_8UC3, cv::Scalar(0, 0, 0));
    image(cv::Rect(20, 20, 2, 2)).setTo(cv::Scalar(255, 255, 255));

    EXPECT_TRUE(detector.IsBlackout(image));
}

}
