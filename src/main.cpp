#include <format>
#include <directxtk/Keyboard.h>

#include "controller/keyboard.hpp"
#include "utils/logger.hpp"
#include "utils/window_inspector.hpp"

namespace logging = sim::utils::logging;
namespace window = sim::utils::window;
using Keys = DirectX::Keyboard::Keys;

int main() {
    logging::init();

    logging::log("Hello, World!");
    logging::log("C++ Version: {} WinVer: {}", __cplusplus, _WIN32_WINNT);

    const window::WindowInspector inspector {};
    const auto window = inspector.Find("Untitled - Notepad");
//    const std::unique_ptr<window::Window> window(std::move(inspector.Find("Untitled - Notepad")));
//    const auto window = window_inspector.Find("Untitled - Notepad");

    const auto& keyboard = sim::controller::Keyboard::Get();
    keyboard.KeyDown({'A', 'B', 'C', Keys::Enter});

    return 0;
}
