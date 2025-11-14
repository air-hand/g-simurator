module;

#include <cstdio>

export module utils:file;

import std;

namespace sim::utils
{

export class FileDeleter final
{
public:
    void operator()(FILE* fp) const;
};

export using FStreamPtr = std::unique_ptr<std::fstream>;
export using FilePtr = std::unique_ptr<FILE, FileDeleter>;

export FStreamPtr open_file(const std::filesystem::path& path, std::ios_base::openmode mode);
export FilePtr open_file(const std::filesystem::path& path, const char* mode);

export bool is_open(const FStreamPtr& stream);
export bool is_open(const FilePtr& fp);

export template<typename StreamPointerT> concept stream_type = requires(StreamPointerT stream)
{
    {
        std::is_same_v<typename std::pointer_traits<StreamPointerT>::element_type::char_type, char>
        ||
        std::is_same_v<typename std::pointer_traits<StreamPointerT>::element_type::char_type, char8_t>
    };
    { stream->seekg(0, std::ios::end) };
    { stream->tellg() } -> std::same_as<std::streampos>;
    { stream->seekg(0, std::ios::beg) };
    { stream->read(nullptr, 0) };
};

export template<
    stream_type StreamPointerT, 
    typename CharT = std::pointer_traits<StreamPointerT>::element_type::char_type
>
std::basic_string<CharT> read_all(StreamPointerT stream)
{
    if (stream == nullptr)
    {
        return {};
    }
    stream->seekg(0, std::ios::end);
    const auto size = stream->tellg();
    stream->seekg(0, std::ios::beg);
    if (size <= 0)
    {
        return {};
    }
    std::basic_string<CharT> content(size, {});
    stream->read(content.data(), size);
    return content;
}

}
