module;

#include "macro.hpp"

export module utils:error;

import std.compat;

namespace sim::utils
{

export class Error final
{
public:
    static Error create(std::string_view message, std::source_location loc = std::source_location::current());
    static Error create(std::string_view message, Error&& cause, std::source_location loc = std::source_location::current());
    static Error create(std::string_view message, std::error_code code, std::source_location loc = std::source_location::current());
    static Error create(std::string_view message, std::error_code code, Error&& cause, std::source_location loc = std::source_location::current());

    DECLARE_MOVE_CONSTRUCTOR_DEFAULT(Error);
    DELETE_COPY_AND_ASSIGN(Error);

    [[nodiscard]] std::string_view message() const noexcept;
    [[nodiscard]] std::error_code code() const noexcept;
    [[nodiscard]] const std::source_location& location() const noexcept;
    [[nodiscard]] const Error* cause() const noexcept;

    [[nodiscard]] std::string str(uint32_t indent = 0) const;
private:
    Error(
        std::string_view message,
        std::error_code code,
        std::unique_ptr<Error> cause,
        std::source_location loc
    );

    std::string message_;
    std::error_code code_;
    std::unique_ptr<Error> cause_;
    std::source_location loc_;
};

export template <typename T>
using Result = std::expected<T, Error>;

}
