#include <directxtk/Keyboard.h>

#include "utils/windows.hpp"
#include "route.hpp"

namespace sim::route
{

using Keys = DirectX::Keyboard::Keys;

void movePractice(const controller::Keyboard& keyboard)
{
    const auto keys = std::array<DirectX::Keyboard::Keys, 4> {
        DirectX::Keyboard::Keys::A,
        DirectX::Keyboard::Keys::B,
        DirectX::Keyboard::Keys::C,
        DirectX::Keyboard::Keys::Enter
    };
}

}
