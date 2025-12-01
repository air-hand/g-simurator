module;

#include "macro.hpp"

export module utils:window_keyboard;

import std.compat;

namespace sim::utils::window
{

using ActivateCallback = std::function<void()>;

export class WindowKeyboard final
{
public:
    explicit WindowKeyboard(ActivateCallback callback) noexcept;
    ~WindowKeyboard();
    WindowKeyboard(WindowKeyboard&&) noexcept;
    WindowKeyboard& operator=(WindowKeyboard&&) noexcept;

    DELETE_COPY_AND_ASSIGN(WindowKeyboard);

    void KeyDown(WORD key) const;
    void KeyUp(WORD key) const;
    void KeyDown(const std::vector<WORD>& keys) const;
    void KeyUp(const std::vector<WORD>& keys) const;
    void KeyPress(const std::vector<WORD>& keys, uint32_t duration_ms) const;
    void KeyPress(WORD key, uint32_t duration_ms) const;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

}
