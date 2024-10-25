export module utils.file;

import std;

//#include "macro.hpp"

namespace sim::utils
{

export class FStreamDeleter final
{
public:
    void operator()(std::fstream* stream) const;
};


export using FStreamPtr = std::unique_ptr<std::fstream, FStreamDeleter>;

export FStreamPtr open_file(const std::filesystem::path& path, std::ios_base::openmode mode);

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
auto read_all(StreamPointerT stream)
{
    std::basic_string<CharT> content;
    if (stream == nullptr)
    {
        return content;
    }
    stream->seekg(0, std::ios::end);
    const auto size = stream->tellg();
    stream->seekg(0, std::ios::beg);
    if (size <= 0)
    {
        return content;
    }
    content.resize(size);
    stream->read(content.data(), size);
    return content;
}

}
