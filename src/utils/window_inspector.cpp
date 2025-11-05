module;

#include <dwmapi.h>

#include "logger.hpp"

module utils;

import std;
import :unicode;
import :logger;

namespace
{

class WindowValidator final
{
public:
    WindowValidator(){}
    ~WindowValidator() = default;

    bool IsCapturable(HWND handle) const
    {
#ifdef DEBUG
        const auto is_window = !!IsWindow(handle);
        const auto is_visible = !!IsWindowVisible(handle);
        const auto is_iconic = !!IsIconic(handle);
        BOOL cloaked = FALSE;
        DwmGetWindowAttribute(handle, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));
        const auto is_cloaked = !!cloaked;

        DWORD affinity = 0;
        GetWindowDisplayAffinity(handle, &affinity);
        const auto is_excluded = (affinity == WDA_EXCLUDEFROMCAPTURE);
        LONG_PTR exstyle = GetWindowLongPtrW(handle, GWL_EXSTYLE);
        const auto no_redirect = (exstyle & WS_EX_NOREDIRECTIONBITMAP) != 0;

        const auto owner = GetWindow(handle, GW_OWNER);

        const auto root = GetAncestor(handle, GA_ROOTOWNER);

        DEBUG_LOG_ARGS(
            R"(
                WindowHandle: 0x{:08x}
                Window: {}
                Visible: {}
                Iconic: {}
                Cloaked: {}
                Affinity: {}
                Excluded: {}
                ExStyle: 0x{:08x}
                NoRedirect: {}
                OwnerHandle: 0x{:08x}
                RootHandle: 0x{:08x}
            )",
            reinterpret_cast<uintptr_t>(handle),
            is_window,
            is_visible,
            is_iconic,
            is_cloaked,
            affinity,
            is_excluded,
            exstyle,
            no_redirect,
            reinterpret_cast<uintptr_t>(owner),
            reinterpret_cast<uintptr_t>(root)
        );
#endif

        if (!IsWindowVisible(handle))
        {
            return false;
        }
        return true;
    }
};

struct DescendantsWindowRange {
    HWND root{};

    struct iterator {
        HWND cur{};
        std::vector<HWND> stack;

        using value_type = HWND;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::input_iterator_tag;
        using iterator_concept  = std::input_iterator_tag;

        value_type operator*() const noexcept { return cur; }

        iterator& operator++() noexcept {
            // 1) 子へ
            if (HWND child = FindWindowExW(cur, nullptr, nullptr, nullptr)) {
                stack.push_back(cur);
                cur = child;
                return *this;
            }
            // 2) 兄弟へ or 親に戻る
            while (!stack.empty()) {
                HWND parent = stack.back();
                if (HWND sibling = FindWindowExW(parent, cur, nullptr, nullptr)) {
                    cur = sibling;
                    return *this;
                }
                cur = parent;
                stack.pop_back();
            }
            // 3) 終了
            cur = nullptr;
            return *this;
        }

        iterator operator++(int) noexcept {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const iterator& other) const noexcept {
            return cur == other.cur;
        }

        bool operator==(std::default_sentinel_t) const noexcept { return cur == nullptr; }
    };

    iterator begin() const noexcept { return { root, {} }; }
    std::default_sentinel_t end() const noexcept { return {}; }
};

}


namespace sim::utils::window
{

class WindowInspector::Impl
{
public:
    Impl() {}
    ~Impl() {}

    std::unique_ptr<Window> Find(const std::string& windowName) const
    {
        const auto windowNameWide = unicode::to_utf16(windowName);
        const auto handle = FindWindowW(NULL, windowNameWide.c_str());
        if (handle == NULL)
        {
            DEBUG_LOG_ARGS("FindWindowW returned NULL for window: {}", windowName);
            return nullptr;
        }

        // Validate the window handle
        if (!IsWindow(handle))
        {
            DEBUG_LOG_ARGS("Invalid window handle for: {}", windowName);
            return nullptr;
        }

        const auto capturable = [](HWND hwnd) -> HWND
        {
            WindowValidator wv;
            for (const auto h : DescendantsWindowRange{hwnd})
            {
                if (wv.IsCapturable(h))
                {
                    return h;
                }
            }
            const auto owner = GetWindow(hwnd, GW_OWNER);
            if (wv.IsCapturable(owner))
            {
                return owner;
            }
            const auto root = GetAncestor(hwnd, GA_ROOTOWNER);
            if (wv.IsCapturable(root))
            {
                return root;
            }
            return NULL;
        }(handle);

        if (capturable == NULL)
        {
            DEBUG_LOG_ARGS("There is no capturable window: {}", windowName);
            return nullptr;
        }

        return std::make_unique<Window>(capturable);
    }
};

WindowInspector::WindowInspector() noexcept
    : impl_(std::make_unique<Impl>())
{
}

WindowInspector::~WindowInspector() = default;

WindowInspector::WindowInspector(WindowInspector&& rhs) noexcept : impl_(std::move(rhs.impl_))
{
}

WindowInspector& WindowInspector::operator=(WindowInspector&& rhs) noexcept
{
    if (this != &rhs)
    {
        impl_ = std::move(rhs.impl_);
    }
    return *this;
}

std::unique_ptr<Window> WindowInspector::Find(const std::string& windowName) const
{
    return impl_->Find(windowName);
}

}
