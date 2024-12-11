module;

#include "proto/route.hpp"
#include "macro.hpp"

module utils;

import :route_validator;

namespace sim::utils
{

namespace route = sim::route;

class RouteValidator::Impl {
private:
    const route::RouteList& r_;
    bool validateROI(const route::ROI& roi) const
    {
        return roi.top_percent() >= 0 && roi.top_percent() <= 100 &&
               roi.left_percent() >= 0 && roi.left_percent() <= 100 &&
               roi.width_percent() > 0 && roi.width_percent() <= 100 &&
               roi.height_percent() > 0 && roi.height_percent() <= 100;
    }

    bool validateRoute(const route::Route& route) const
    {
        return !route.expected().empty() &&
               route.has_roi() && validateROI(route.roi()) &&
               !route.keys().empty();
    }

    bool validateRouteList(const route::RouteList& list) const
    {
        return !list.name().empty() &&
               !list.window_name().empty() &&
               !list.routes().empty() &&
               std::ranges::all_of(list.routes(), validateRoute);
    }

public:
    Impl(const route::RouteList& r) : r_(r)
    {
    }
    ~Impl() = default;

    bool Validate() const
    {
        return validateRouteList(r_);
    }
};

RouteValidator::RouteValidator(const route::RouteList& r) noexcept
    : impl_(std::make_unique<Impl>(r))
{
}

RouteValidator::~RouteValidator() = default;

bool RouteValidator::Validate() const
{
    return impl_->Validate();
}

} // namespace
