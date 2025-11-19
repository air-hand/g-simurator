module;

#include "macro.hpp"

export module utils:window_keyboard;

import std;

namespace sim::utils::window
{

export class WindowKeyboard final
{
public:
    explicit WindowKeyboard(HWND hwnd) noexcept;
    ~WindowKeyboard();
    WindowKeyboard(WindowKeyboard&&) noexcept;
    WindowKeyboard& operator=(WindowKeyboard&&) noexcept;

    DELETE_COPY_AND_ASSIGN(WindowKeyboard);

    void KeyDown(WORD key) const;
    void KeyUp(WORD key) const;
    void KeyDown(const std::vector<WORD>& keys) const;
    void KeyUp(const std::vector<WORD>& keys) const;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

}
