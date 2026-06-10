module;

#include <opencv2/opencv.hpp>
#include <opencv2/core/cuda.hpp>

export module utils:image;

import std;
import :algorithm;

namespace sim::utils::image
{

export struct RelativeRect
{
    double x;
    double y;
    double width;
    double height;
};

export inline cv::Rect toRect(const RelativeRect& roi, const cv::Size& size)
{
    const auto normalize = [](const double value) {
        if (!std::isfinite(value))
        {
            return 0.0;
        }
        return sim::utils::algorithm::safe_clamp(value, 0.0, 1.0);
    };

    const auto left = normalize(roi.x);
    const auto top = normalize(roi.y);
    const auto right = sim::utils::algorithm::safe_clamp(normalize(roi.x + roi.width), left, 1.0);
    const auto bottom = sim::utils::algorithm::safe_clamp(normalize(roi.y + roi.height), top, 1.0);

    const auto image_width = std::max(size.width, 0);
    const auto image_height = std::max(size.height, 0);

    const auto x1 = sim::utils::algorithm::safe_clamp(static_cast<int>(left * image_width), 0, image_width);
    const auto y1 = sim::utils::algorithm::safe_clamp(static_cast<int>(top * image_height), 0, image_height);
    const auto x2 = sim::utils::algorithm::safe_clamp(static_cast<int>(right * image_width), x1, image_width);
    const auto y2 = sim::utils::algorithm::safe_clamp(static_cast<int>(bottom * image_height), y1, image_height);

    return cv::Rect(x1, y1, x2 - x1, y2 - y1);
}

export template<typename T>
cv::Rect toRect(const RelativeRect& roi, const T& input)
{
    return toRect(roi, cv::Size(input.cols, input.rows));
}

export template<typename T>
T cropView(const T& input, const cv::Rect& roi)
{
    return input(roi);
}

export template<typename T>
T cropView(const T& input, const RelativeRect& roi)
{
    return cropView(input, toRect(roi, input));
}

export template<typename T>
T crop(const T& input, const cv::Rect& roi)
{
    return cropView(input, roi).clone();
}

export template<typename T>
T crop(const T& input, const RelativeRect& roi)
{
    return cropView(input, roi).clone();
}

export cv::cuda::GpuMat grayScale(const cv::cuda::GpuMat& input);
export cv::cuda::GpuMat threshold(const cv::cuda::GpuMat& input);
export cv::cuda::GpuMat resize(const cv::cuda::GpuMat& input, double scale_width, double scale_height);

export cv::Mat fromGPU(const cv::cuda::GpuMat& input);

export cv::Mat grayScale(const cv::Mat& input);

export void saveImage(const cv::Mat& image, const std::string& filename);
export inline void saveImage(const cv::Mat& image, const std::filesystem::path& filename)
{
    saveImage(image, filename.string());
}

}
