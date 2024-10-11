#pragma once

#include <filesystem>
#include <memory>
#include <fstream>

namespace sim::utils
{

class FileDeleter final
{
public:
    void operator()(FILE* file) const;
};

class FStreamDeleter final
{
public:
    void operator()(std::fstream* stream) const;
};


using FilePtr = std::unique_ptr<FILE, FileDeleter>;
using FStreamPtr = std::unique_ptr<std::fstream, FStreamDeleter>;

FStreamPtr open_file(const std::filesystem::path& path, std::ios_base::openmode mode);

}
