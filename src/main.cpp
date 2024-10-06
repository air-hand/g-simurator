#include <format>

#include "controller/keyboard.hpp"
#include "utils/logger.hpp"
#include "utils/window_inspector.hpp"

namespace logging = sim::utils::logging;
namespace window = sim::utils::window;

int main() {
    logging::init();

    logging::log("Hello, World!");
    logging::log(std::format("C++ Version: {} WinVer: {}", __cplusplus, _WIN32_WINNT));

    const window::WindowInspector inspector {};
    inspector.Find("Untitled - Notepad");
//    const std::unique_ptr<window::Window> window(std::move(inspector.Find("Untitled - Notepad")));
//    const auto window = window_inspector.Find("Untitled - Notepad");

    const auto& keyboard = sim::controller::Keyboard::Get();
    keyboard.KeyDown({'A', 'B', 'C'});

    return 0;
}
