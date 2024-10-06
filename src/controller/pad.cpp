#include <memory>

// https://github.com/microsoft/DirectXTK/wiki/Game-controller-input

#include <directxtk/GamePad.h>

void foobar() {
    std::unique_ptr<DirectX::GamePad> pad;
}
