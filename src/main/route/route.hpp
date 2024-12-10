#pragma once

#include <opencv2/opencv.hpp>

#include "proto/route.hpp"
#include "utils/macro.hpp"

import std;

namespace sim::route
{

class RouteReader final
{
public:
    RouteReader() noexcept;
    ~RouteReader();
    RouteReader(RouteReader&&) noexcept;
    RouteReader& operator=(RouteReader&&);

    DELETE_COPY_AND_ASSIGN(RouteReader);

    RouteList ReadJSONFile(const std::filesystem::path& path) const;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

cv::Mat applyROI(const cv::Mat& input, const ROI& roi);

}
