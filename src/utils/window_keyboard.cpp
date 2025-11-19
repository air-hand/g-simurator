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
        // 全てのキーをDownする（同時押し対応）
        for (const auto key : keys)
        {
            PostMessageW(hwnd_, WM_KEYDOWN, key, 0);
        }
    }

    void KeyUp(const std::vector<WORD>& keys) const
    {
        // 全てのキーをUpする（逆順）
        for (const auto key : keys | std::views::reverse)
        {
            PostMessageW(hwnd_, WM_KEYUP, key, 0);
        }
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
