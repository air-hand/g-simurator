#include <fstream>

#include "file.hpp"

namespace sim::utils
{

void FileDeleter::operator()(FILE* file) const
{
    fclose(file);
}

void FStreamDeleter::operator()(std::fstream* stream) const
{
    stream->close();
    delete stream;
}

FStreamPtr open_file(const std::filesystem::path& path, std::ios_base::openmode mode)
{
    return FStreamPtr(new std::fstream(path, mode));
}

std::string read_file(FStreamPtr stream)
{
    std::string content;
    stream->seekg(0, std::ios::end);
    const auto size = stream->tellg();
    content.resize(size, '\0');
    stream->seekg(0, std::ios::beg);
    stream->read(content.data(), size);
    return std::move(content);
}

}
