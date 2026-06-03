module;

module utils;

import std.compat;
import :error;

namespace sim::utils
{

Error Error::create(std::string_view message, std::source_location loc)
{
    return Error(
        message,
        {},
        nullptr,
        loc
    );
}

Error Error::create(std::string_view message, Error&& cause, std::source_location loc)
{
    return Error(
        message,
        {},
        std::make_unique<Error>(std::move(cause)),
        loc
    );
}

Error Error::create(std::string_view message, std::error_code code, std::source_location loc)
{
    return Error(
        message,
        code,
        nullptr,
        loc
    );
}

Error Error::create(std::string_view message, std::error_code code, Error&& cause, std::source_location loc)
{
    return Error(
        message,
        code,
        std::make_unique<Error>(std::move(cause)),
        loc
    );
}

std::string_view Error::message() const noexcept
{
    return message_;
}

std::error_code Error::code() const noexcept
{
    return code_;
}

const std::source_location& Error::location() const noexcept
{
    return loc_;
}

const Error* Error::cause() const noexcept
{
    return cause_.get();
}

std::string Error::str(uint32_t indent) const
{
    std::string out;
    out.reserve(255);

    out.append(indent, ' ');
    std::format_to(std::back_inserter(out), "error: {}{}", message_, '\n');
    if (code_)
    {
        out.append(indent + 1, ' ');
        std::format_to(std::back_inserter(out), "code: {}{}", code_.message(), '\n');
    }
    out.append(indent + 1, ' ');
    std::format_to(std::back_inserter(out), "at {}:{} in {}{}", loc_.file_name(), loc_.line(), loc_.function_name(), '\n');
    if (cause_)
    {
        out.append(indent + 2, ' ');
        out += "caused by:\n";
        out += cause_->str(indent+3);
    }
    return out;
}

Error::Error(
    std::string_view message,
    std::error_code code,
    std::unique_ptr<Error> cause,
    std::source_location loc
): message_(message), code_(code), cause_(std::move(cause)), loc_(loc)
{}

}
