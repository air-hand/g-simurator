module;

#include "proto/route.hpp"
#include "macro.hpp"

module utils;

import std;
import :route_validator;

namespace sim::utils
{

namespace route = sim::route;

class RouteValidator::Impl
{
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
        return !route.expected().empty()
                && route.has_roi() && validateROI(route.roi())
                && !route.keys().empty() && std::ranges::all_of(route.keys(), [](const auto& k) { return !k.empty(); })
                && route.keypress_duration_ms() >= 0
                ;
    }

    bool validateRouteList(const route::RouteList& list) const
    {
        return !list.name().empty() &&
               !list.window_name().empty() &&
               !list.routes().empty() &&
               std::ranges::all_of(list.routes(), [this](const route::Route& r) { return validateRoute(r); });
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

    DELETE_COPY_AND_ASSIGN(Impl);
};

RouteValidator::RouteValidator(const route::RouteList& r) noexcept
    : impl_(std::make_unique<Impl>(r))
{
}

RouteValidator::~RouteValidator() = default;

bool RouteValidator::operator()() const
{
    return impl_->Validate();
}

} // namespace
