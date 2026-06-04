module;

#include "std/windows.hpp"

#define STR1(x) #x
#define STR(x)  STR1(x)

// Pass by CMake
#ifndef SIMURATOR_BUILD_COMMIT
#error "SIMURATOR_BUILD_COMMIT is not defined"
#endif

module sim;

import :build_info;

static_assert(__cplusplus == 202400L, "C++23 required (/std:c++23preview + /Zc:__cplusplus) Current: " STR(__cplusplus));
static_assert(_MSC_VER == 1944, "MSVC 19.44 required. Current: " STR(_MSC_VER));
static_assert(_WIN32_WINNT == 0x0A00, "Windows 10 or 11 required. Current: " STR(_WIN32_WINNT));

namespace sim
{

std::string_view BuildCommit() noexcept
{
    constexpr std::string_view commit = SIMURATOR_BUILD_COMMIT;
    static_assert(!commit.empty(), "SIMURATOR_BUILD_COMMIT must not be empty");
    return commit;
}

}
