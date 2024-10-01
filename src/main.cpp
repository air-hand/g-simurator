#include <format>

#include "utils/logger.hpp"
#include "controller/keyboard.hpp"

namespace logging = sim::utils::logging;

int main() {
    logging::init();

    logging::log("Hello, World!");
    logging::log(std::format("C++ Version: {} WinVer: {}", __cplusplus, _WIN32_WINNT));

    const auto& keyboard = sim::controller::Keyboard::Get();
    keyboard.KeyDown({'A', 'B', 'C'});

    return 0;
}
