#pragma once

#include <memory>

namespace sim::utils
{

class FileDeleter;

using FilePtr = std::unique_ptr<FILE, FileDeleter>;

}
