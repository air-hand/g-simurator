#pragma once

#include "macro.hpp"

namespace sim::utils
{

class Capture final
{
public:
    UTILS_EXPORT ~Capture();
    DELETE_COPY_AND_ASSIGN(Capture);

    UTILS_EXPORT static Capture& Get() noexcept;

    UTILS_EXPORT void Init() const noexcept;
    UTILS_EXPORT void Finalize() const noexcept;

private:
    Capture() noexcept;
};

}
