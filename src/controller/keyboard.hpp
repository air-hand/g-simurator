#pragma once

#include <memory>
#include <vector>

#include "../utils/windows.hpp"

namespace sim
{
namespace controller
{

class Keyboard final
{
public:
    Keyboard() noexcept(false);
    Keyboard(Keyboard&&) noexcept;
    Keyboard& operator=(Keyboard&&) noexcept;

    Keyboard(Keyboard const&) = delete;
    Keyboard& operator=(Keyboard const&) = delete;

    ~Keyboard();

    static const Keyboard& Get();

    void KeyDown(const std::vector<WORD>& keys) const;
    void KeyDown(WORD key) const;
    void KeyUp(const std::vector<WORD>& keys) const;
    void KeyUp(WORD key) const;
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

}
}
