#include "keyboard.hpp"

namespace
{
const auto keyboard = sim::controller::Keyboard();
}

namespace sim::controller
{

class Keyboard::Impl
{
private:
    void SendKeys(const std::vector<WORD>& keys, DWORD flags) {
        std::vector<INPUT> inputs;
        inputs.reserve(keys.size());
        for (const auto key : keys) {
            INPUT input = {};
            input.type = INPUT_KEYBOARD;
            input.ki.wVk = key;
            input.ki.dwFlags = flags;
            inputs.emplace_back(input);
        }
        SendInput(inputs.size(), inputs.data(), sizeof(INPUT));
    }
public:
    Impl() {}
    ~Impl() {}

    void KeyDown(const std::vector<WORD>& keys) {
        SendKeys(keys, 0);
    }

    void KeyUp(const std::vector<WORD>& keys) {
        SendKeys(keys, KEYEVENTF_KEYUP);
    }
};

Keyboard::Keyboard() noexcept(false)
    : impl_(std::make_unique<Impl>())
{
}

Keyboard::Keyboard(Keyboard&& other) noexcept
    : impl_(std::move(other.impl_))
{
}

Keyboard::~Keyboard()
{
}

const Keyboard& Keyboard::Get() {
    return keyboard;
}

void Keyboard::KeyDown(const std::vector<WORD>& keys) const {
    impl_->KeyDown(keys);
}

void Keyboard::KeyDown(WORD key) const {
    impl_->KeyDown({key});
}

void Keyboard::KeyUp(const std::vector<WORD>& keys) const {
    impl_->KeyUp(keys);
}

void Keyboard::KeyUp(WORD key) const {
    impl_->KeyUp({key});
}

}
