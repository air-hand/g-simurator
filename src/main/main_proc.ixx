module;

#include "utils/macro.hpp"

export module sim:main;

import std.compat;

namespace sim
{

export class MainProc final
{
public:
    MainProc(uint32_t argc, char** argv) noexcept;
    ~MainProc();
    DECLARE_MOVE_CONSTRUCTOR(MainProc);

    DELETE_COPY_AND_ASSIGN(MainProc);

    uint32_t Run();
    void Cancel();
    void AddFinalizer(std::function<void()> finalizer);
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};
}
