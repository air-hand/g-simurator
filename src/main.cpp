#include <directxtk/Keyboard.h>

#include "controller/keyboard.hpp"
#include "utils/logger.hpp"
#include "utils/window_inspector.hpp"

namespace logging = sim::utils::logging;
namespace window = sim::utils::window;
using Keys = DirectX::Keyboard::Keys;

int main() {
    logging::init();

    logging::log(L"Hello, World!");
    logging::log("C++ Version: {} WinVer: {}", __cplusplus, _WIN32_WINNT);

    const auto desktop = window::Window(GetDesktopWindow());
    desktop.Activate();
    desktop.Capture();

    constexpr const auto WINDOW_TITLE = L"TEKKEN™8  ";
    const window::WindowInspector inspector {};
    const auto window = inspector.Find(WINDOW_TITLE);
    if (window == nullptr) {
        logging::log(L"Window not found: {}", WINDOW_TITLE);
        return 1;
    }
    window->Activate();

    const auto& keyboard = sim::controller::Keyboard::Get();
    keyboard.KeyDown({'A', 'B', 'C', Keys::Enter});

    window->Capture();

    logging::log(L"Good bye, {}", L"World...");
    return 0;
}
