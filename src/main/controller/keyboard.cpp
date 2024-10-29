#include "keyboard.hpp"

#include "std/windows.hpp"

import std.compat;

namespace sim::controller
{

class Keyboard::Impl
{
private:
    void SendKeys(const std::vector<WORD>& keys, DWORD flags)
    {
        std::vector<INPUT> inputs;
        inputs.reserve(keys.size());
        for (const auto key : keys)
        {
            INPUT input = {};
            input.type = INPUT_KEYBOARD;
            input.ki.wVk = key;
            input.ki.dwFlags = flags;
            inputs.emplace_back(input);
        }
        SendInput(static_cast<uint64_t>(inputs.size()), inputs.data(), sizeof(INPUT));
    }
public:
    Impl() {}
    ~Impl() {}

    void KeyDown(const std::vector<WORD>& keys)
    {
        SendKeys(keys, 0);
    }

    void KeyUp(const std::vector<WORD>& keys)
    {
        SendKeys(keys, KEYEVENTF_KEYUP);
    }
};

Keyboard::Keyboard() noexcept
    : impl_(std::make_unique<Impl>())
{
}

Keyboard::Keyboard(Keyboard&& rhs) noexcept
    : impl_(std::move(rhs.impl_))
{
}

Keyboard& Keyboard::operator=(Keyboard&& rhs) noexcept
{
    if (this != &rhs)
    {
        impl_ = std::move(rhs.impl_);
    }
    return *this;
}

const Keyboard& Keyboard::Get()
{
    static const auto keyboard = Keyboard();
    return keyboard;
}

void Keyboard::KeyDown(const std::vector<WORD>& keys) const
{
    impl_->KeyDown(keys);
}

void Keyboard::KeyDown(WORD key) const
{
    impl_->KeyDown({key});
}

void Keyboard::KeyUp(const std::vector<WORD>& keys) const
{
    impl_->KeyUp(keys);
}

void Keyboard::KeyUp(WORD key) const
{
    impl_->KeyUp({key});
}

}
