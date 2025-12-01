module;

#include "logger.hpp"

module utils;

import :logger;

namespace sim::utils
{

void FileDeleter::operator()(FILE* fp) const
{
    DEBUG_LOG_SPAN(_);
    if (fp)
    {
        std::fclose(fp);
    }
}

FStreamPtr open_file(const std::filesystem::path& path, std::ios_base::openmode mode)
{
    DEBUG_LOG_ARGS("open file: {}", path.string());
    return FStreamPtr(new std::fstream(path, mode));
}

FilePtr open_file(const std::filesystem::path& path, const char* mode)
{
    DEBUG_LOG_ARGS("open file: {}", path.string());
#pragma warning(suppress: 4996)
    return FilePtr(std::fopen(path.string().c_str(), mode));
}

}
