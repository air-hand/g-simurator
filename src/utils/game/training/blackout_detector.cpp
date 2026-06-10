module;

#include <opencv2/opencv.hpp>

module utils;

import :game.training.blackout_detector;
import :image;

namespace sim::utils::game::training
{

class BlackoutDetector::Impl
{
public:
    Impl(BlackoutDetectorConfig config) : config_(config) {}
    ~Impl() = default;

    bool IsBlackout(const cv::Mat& image) const
    {
        const auto target = image::cropView(image, config_.roi);
        const auto gray = image::grayScale(target);
        const auto bright = gray > config_.brightLumaThreshold;
        const auto brightRatio = static_cast<double>(cv::countNonZero(bright)) / static_cast<double>(gray.total());
        return brightRatio <= config_.maxBrightPixelRatio;
    }
private:
    BlackoutDetectorConfig config_;
};

BlackoutDetector::BlackoutDetector(BlackoutDetectorConfig config) noexcept : impl_(std::make_unique<Impl>(config)) {}
BlackoutDetector::~BlackoutDetector() = default;
BlackoutDetector::BlackoutDetector(BlackoutDetector&&) noexcept = default;
BlackoutDetector& BlackoutDetector::operator=(BlackoutDetector&&) noexcept = default;

bool BlackoutDetector::IsBlackout(const cv::Mat& image) const
{
    return impl_->IsBlackout(image);
}

BlackoutSpanBoundaryDetector::BlackoutSpanBoundaryDetector(BlackoutDetectorConfig config) : detector_(config) {}

bool BlackoutSpanBoundaryDetector::operator()(const utils::CapturedImage& img) const
{
    return detector_.IsBlackout(img.Read());
}

}
