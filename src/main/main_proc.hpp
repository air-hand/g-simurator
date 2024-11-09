#pragma once

#include "utils/macro.hpp"

import std.compat;

namespace sim
{

class MainProc final
{
public:
    MainProc(uint32_t argc, char** argv) noexcept;
    ~MainProc();
    MainProc(MainProc&&) noexcept;
    MainProc& operator=(MainProc&&);

    DELETE_COPY_AND_ASSIGN(MainProc);

    int32_t Run();
    void AddFinalizer(std::function<void()> finalizer);
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};
}
