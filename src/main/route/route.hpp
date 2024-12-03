#pragma once

#include "proto/gen/route.pb.h"
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

    Route ReadJSONFile(const std::filesystem::path& path) const;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

}
