module;

#include "utils/macro.hpp"

export module main.main_proc;

import std;

namespace sim
{

export class MainProc final
{
public:
    MainProc(int argc, char** argv) noexcept;
    ~MainProc();
    MainProc(MainProc&&) noexcept;
    MainProc& operator=(MainProc&&);

    DELETE_COPY_AND_ASSIGN(MainProc);

    int Run();
    void AddFinalizer(std::function<void()> finalizer);
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};
}
