module;

#include "proto/gen/route.pb.h"

#include "utils/macro.hpp"

export module main.route.route;

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

    Route ReadJSONFile(const std::filesystem::path& path) const;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

}
