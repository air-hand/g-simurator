module;

module sim;

import std.compat;
import :keyboard;

namespace
{
// https://learn.microsoft.com/ja-jp/windows/win32/inputdev/virtual-key-codes
WORD key_name_to_vk(const std::string& name)
{
    static const std::unordered_map<std::string, WORD> table{
        {"CTRL",   VK_CONTROL},
        {"SHIFT",  VK_SHIFT},
        {"ALT",    VK_MENU},
        {"LCTRL",  VK_LCONTROL},
        {"RCTRL",  VK_RCONTROL},
        {"LSHIFT", VK_LSHIFT},
        {"RSHIFT", VK_RSHIFT},
        {"LALT",   VK_LMENU},
        {"RALT",   VK_RMENU},

        {"ENTER",  VK_RETURN},
        {"ESC",    VK_ESCAPE},
        {"TAB",    VK_TAB},
        {"SPACE",  VK_SPACE},
        {"LEFT",   VK_LEFT},
        {"RIGHT",  VK_RIGHT},
        {"UP",     VK_UP},
        {"DOWN",   VK_DOWN},

        {"F1",  VK_F1},  {"F2",  VK_F2},  {"F3",  VK_F3},  {"F4",  VK_F4},
        {"F5",  VK_F5},  {"F6",  VK_F6},  {"F7",  VK_F7},  {"F8",  VK_F8},
        {"F9",  VK_F9},  {"F10", VK_F10}, {"F11", VK_F11}, {"F12", VK_F12},
    };

    auto it = table.find(name);
    if (it != table.end()) {
        return it->second;
    }

    if (name.size() != 1) {
        DEBUG_ASSERT(false);
        return std::nullopt;
    }

        const auto ch = static_cast<unsigned char>(name.at(0));

        SHORT r = VkKeyScanA(ch);
        if (r == -1) {
            DEBUG_LOG_ARGS("VkKeyScanA failed for char: {}" + name);
            DEBUG_ASSERT(false);
            return std::nullopt;
        }
        return LOBYTE(r);
}
}

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
