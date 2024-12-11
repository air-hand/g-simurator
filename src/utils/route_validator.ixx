module;

#include "proto/route.hpp"
#include "macro.hpp"

export module utils:route_validator;

import std;

namespace sim::utils
{

namespace route = sim::route;

export class RouteValidator final {
public:
    explicit RouteValidator(const route::RouteList& r) noexcept;
    ~RouteValidator();

    DELETE_COPY_AND_ASSIGN(RouteValidator);

    bool Validate() const;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace
