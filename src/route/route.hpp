#pragma once

#include <string>

#include "controller/keyboard.hpp"

namespace sim::route
{

class Route final
{
public:
    Route() noexcept;
    ~Route();
    Route(Route&&) noexcept;
    Route& operator=(Route&&) noexcept;

    DELETE_COPY_AND_ASSIGN(Route);

    const std::string& Name() const noexcept;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

class RouteDetector final
{
public:
    RouteDetector() noexcept;
    ~RouteDetector();
    RouteDetector(RouteDetector&&) noexcept;
    RouteDetector& operator=(RouteDetector&&) noexcept;

    DELETE_COPY_AND_ASSIGN(RouteDetector);

    std::unique_ptr<Route> SelectRoute(const std::string& name);

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

}
