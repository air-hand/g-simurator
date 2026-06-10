module;

#include "macro.hpp"

export module utils:console_stop;

import std.compat;

namespace sim::utils
{

export class ConsoleStopHandler final
{
public:
    ConsoleStopHandler();
    ~ConsoleStopHandler();

    DELETE_COPY_AND_ASSIGN(ConsoleStopHandler);
    DELETE_MOVE_AND_ASSIGN(ConsoleStopHandler);

    [[nodiscard]] std::stop_token get_token() const noexcept;
    [[nodiscard]] bool stop_requested() const noexcept;

    bool request_stop() noexcept;
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

}
