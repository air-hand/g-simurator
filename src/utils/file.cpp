import utils.file;

namespace sim::utils
{

void FStreamDeleter::operator()(std::fstream* stream) const
{
    stream->close();
    delete stream;
}

FStreamPtr open_file(const std::filesystem::path& path, std::ios_base::openmode mode)
{
    if (!std::filesystem::exists(path))
    {
        return nullptr;
    }
    return FStreamPtr(new std::fstream(path, mode));
}

}
