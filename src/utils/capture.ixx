//module;

#include "macro.hpp"

export module utils.capture;

namespace sim::utils
{

export class Capture final
{
public:
    ~Capture();
    DELETE_COPY_AND_ASSIGN(Capture);

    static Capture& Get() noexcept;

    void Init() const noexcept;
    void Finalize() const noexcept;

private:
    Capture() noexcept;
};

}
