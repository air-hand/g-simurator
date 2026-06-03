module;

#include <opencv2/opencv.hpp>

#include "utils/macro.hpp"

export module utils:game.training.blackout_detector;

import std;

import :capture;

namespace utils = sim::utils;

namespace sim::utils::game::training
{

export struct BlackoutDetectorConfig
{
    cv::Rect roi;
    double brightLumaThreshold = 32.0;
    double maxBrightPixelRatio = 0.001;
};

export class BlackoutDetector final
{
public:
    explicit BlackoutDetector(BlackoutDetectorConfig config) noexcept;
    ~BlackoutDetector();
    DECLARE_MOVE_CONSTRUCTOR(BlackoutDetector);

    DELETE_COPY_AND_ASSIGN(BlackoutDetector);

    bool IsBlackout(const cv::Mat& image) const;
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

export class BlackoutSpanBoundaryDetector final
{
public:
    explicit BlackoutSpanBoundaryDetector(BlackoutDetectorConfig config);
    ~BlackoutSpanBoundaryDetector() = default;
    DECLARE_MOVE_CONSTRUCTOR_DEFAULT(BlackoutSpanBoundaryDetector);
    DELETE_COPY_AND_ASSIGN(BlackoutSpanBoundaryDetector);

    bool operator()(const utils::CapturedImage& img) const;
private:
    BlackoutDetector detector_;
};

}
