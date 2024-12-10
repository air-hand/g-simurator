#pragma once

#pragma warning(push)
#pragma warning(disable : 4946)
#include "proto/gen/route.pb.h"
#pragma warning(pop)
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

}
