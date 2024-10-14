#pragma once

#include <functional>
#include <memory>

#include "../utils/macro.hpp"

namespace sim
{

class MainProc final
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
