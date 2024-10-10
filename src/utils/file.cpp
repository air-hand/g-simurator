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

FStreamPtr open_file(std::fstream* stream)
{
    return FStreamPtr(stream);
}
}
