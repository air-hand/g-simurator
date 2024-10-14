#pragma once

#include <filesystem>
#include <memory>
#include <fstream>
#include <sstream>
#include <streambuf>

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

std::string read_file(FStreamPtr stream);

template<typename StreamPointerT> concept stream_type = requires(StreamPointerT stream)
{
    { std::is_pointer_v<StreamPointerT> == true };
    {
        std::is_same_v<typename std::remove_pointer_t<decltype(stream)>::char_type, char>
        || 
        std::is_same_v<typename std::remove_pointer_t<decltype(stream)>::char_type, char8_t>
    };
    { stream->seekg(0, std::ios::end) };
    { stream->tellg() } -> std::same_as<std::streampos>;
    { stream->seekg(0, std::ios::beg) };
    { stream->read(nullptr, 0) };
};

template<stream_type StreamPointerT>
auto read_all(StreamPointerT stream)
{
    using CharT = std::remove_pointer_t<decltype(stream)>::char_type;
    std::basic_string<CharT> content;
    if (stream == nullptr)
    {
        return std::move(content);
    }
    stream->seekg(0, std::ios::end);
    const auto size = stream->tellg();
    stream->seekg(0, std::ios::beg);
    content.resize(size, CharT('\0'));
    stream->read(content.data(), size);
    return std::move(content);
}

}
