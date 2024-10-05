#include "file.hpp"

namespace sim::utils
{
class FileDeleter final
{
public:
    void operator()(FILE* file) const
    {
        fclose(file);
    }
};
}
