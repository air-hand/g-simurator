module;

#include <opencv2/opencv.hpp>

#include "std/windows.hpp"
#include "proto/route.hpp"
#include "utils/macro.hpp"

export module sim:route;

import std;

namespace sim::route
{

export class RouteReader final
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

export cv::Mat applyROI(const cv::Mat& input, const ROI& roi);
export std::vector<WORD> keys(const route::Route& r);

}
