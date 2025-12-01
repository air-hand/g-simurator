module;

module utils;

import :window_keyboard;
import :time;

namespace sim::utils::window
{

class WindowKeyboard::Impl
{
private:
    ActivateCallback callback_;

    template<typename R>
        requires std::ranges::range<R>
    void KeyImpl(R&& keys, DWORD keyevent_bit) const
    {
        if (callback_) {
            callback_();
        }
        std::vector<INPUT> inputs;
        inputs.reserve(std::ranges::size(keys));

        for (const auto key : keys)
        {
            INPUT input = {};
            input.type = INPUT_KEYBOARD;
            input.ki.wVk = key;
            input.ki.wScan = static_cast<WORD>(MapVirtualKeyW(key, MAPVK_VK_TO_VSC));
            input.ki.dwFlags = KEYEVENTF_SCANCODE | keyevent_bit;
            inputs.emplace_back(input);
        }

        SendInput(static_cast<UINT>(inputs.size()), inputs.data(), sizeof(INPUT));
    }

public:
    explicit Impl(ActivateCallback callback) noexcept
        : callback_(callback)
    {
    }

    void KeyDown(const std::vector<WORD>& keys) const
    {
        KeyImpl(keys, 0);
    }

    void KeyUp(const std::vector<WORD>& keys) const
    {
        // KeyImpl(keys | std::views::reverse, KEYEVENTF_KEYUP);
        KeyImpl(keys, KEYEVENTF_KEYUP);
    }
};

WindowKeyboard::WindowKeyboard(ActivateCallback callback) noexcept
    : impl_(std::make_unique<Impl>(callback))
{
}

WindowKeyboard::~WindowKeyboard() = default;

WindowKeyboard::WindowKeyboard(WindowKeyboard&&) noexcept = default;

WindowKeyboard& WindowKeyboard::operator=(WindowKeyboard&& rhs) noexcept
{
    if (this != &rhs)
    {
        impl_ = std::move(rhs.impl_);
    }
    return *this;
}

void WindowKeyboard::KeyDown(WORD key) const
{
    impl_->KeyDown({key});
}

void WindowKeyboard::KeyUp(WORD key) const
{
    impl_->KeyUp({key});
}

void WindowKeyboard::KeyDown(const std::vector<WORD>& keys) const
{
    impl_->KeyDown(keys);
}

void WindowKeyboard::KeyUp(const std::vector<WORD>& keys) const
{
    impl_->KeyUp(keys);
}

void WindowKeyboard::KeyPress(const std::vector<WORD>& keys, uint32_t duration_ms) const
{
    impl_->KeyDown(keys);
    sim::utils::time::sleep(duration_ms);
    impl_->KeyUp(keys);
}

void WindowKeyboard::KeyPress(WORD key, uint32_t duration_ms) const
{
    impl_->KeyDown({key});
    sim::utils::time::sleep(duration_ms);
    impl_->KeyUp({key});
}

}
