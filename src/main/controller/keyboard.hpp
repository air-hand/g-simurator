#pragma once

#include <memory>
#include <vector>

#include "utils/macro.hpp"
#include "utils/windows.hpp"

namespace sim::controller
{

class Keyboard final
{
public:
    static const Keyboard& Get();

    ~Keyboard() = default;

    void KeyDown(const std::vector<WORD>& keys) const;
    void KeyDown(WORD key) const;
    void KeyUp(const std::vector<WORD>& keys) const;
    void KeyUp(WORD key) const;

private:
    Keyboard() noexcept;
    Keyboard(Keyboard&&) noexcept;
    Keyboard& operator=(Keyboard&&) noexcept;

    DELETE_COPY_AND_ASSIGN(Keyboard);

    class Impl;
    std::unique_ptr<Impl> impl_;
};

}
