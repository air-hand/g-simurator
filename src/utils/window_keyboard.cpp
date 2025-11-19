module;

module utils;

import std;
import :window_keyboard;

namespace sim::utils::window
{

class WindowKeyboard::Impl
{
public:
    explicit Impl(HWND hwnd) noexcept
        : hwnd_(hwnd)
    {
    }

    void KeyDown(const std::vector<WORD>& keys) const
    {
        std::vector<INPUT> inputs;
        inputs.reserve(keys.size());

        // 全てのキーをDownする（同時押し対応）
        for (const auto key : keys)
        {
            INPUT input = {};
            input.type = INPUT_KEYBOARD;
            input.ki.wVk = key;
            input.ki.wScan = static_cast<WORD>(MapVirtualKeyW(key, MAPVK_VK_TO_VSC));
            input.ki.dwFlags = KEYEVENTF_SCANCODE;
            inputs.emplace_back(input);
        }

        SendInput(static_cast<UINT>(inputs.size()), inputs.data(), sizeof(INPUT));
    }

    void KeyUp(const std::vector<WORD>& keys) const
    {
        std::vector<INPUT> inputs;
        inputs.reserve(keys.size());

        // 全てのキーをUpする（逆順）
        for (const auto key : keys | std::views::reverse)
        {
            INPUT input = {};
            input.type = INPUT_KEYBOARD;
            input.ki.wVk = key;
            input.ki.wScan = static_cast<WORD>(MapVirtualKeyW(key, MAPVK_VK_TO_VSC));
            input.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
            inputs.emplace_back(input);
        }

        SendInput(static_cast<UINT>(inputs.size()), inputs.data(), sizeof(INPUT));
    }

private:
    HWND hwnd_;
};

WindowKeyboard::WindowKeyboard(HWND hwnd) noexcept
    : impl_(std::make_unique<Impl>(hwnd))
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

}
