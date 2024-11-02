module;

#include "logger.hpp"

module utils;

import :logger;

namespace sim::utils
{

void FStreamDeleter::operator()(std::fstream* stream) const
{
    DEBUG_LOG_SPAN(_);
    stream->close();
    delete stream;
}

FStreamPtr open_file(const std::filesystem::path& path, std::ios_base::openmode mode)
{
    DEBUG_LOG_ARGS("open file: {}", path.string());
    if (!std::filesystem::exists(path))
    {
        return nullptr;
    }
    return FStreamPtr(new std::fstream(path, mode));
}

}
