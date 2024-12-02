module;

#include <opencv2/opencv.hpp>
#include <opencv2/core/cuda.hpp>

#include "macro.hpp"

export module utils:image;

import std;

namespace sim::utils::image
{

#if 0
class ImageManipulator final
{
public:
    ImageManipulator() noexcept;
    ~ImageManipulator() noexcept;
    DELETE_COPY_AND_ASSIGN(ImageManipulator);

    void Crop(cv::cuda::GpuMat& image, const cv::Rect& roi) const;
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};
#endif

}
