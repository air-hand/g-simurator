include("${VCPKG_ROOT_DIR}/triplets/x64-windows.cmake")

# /permissive for protobuf.
#set(VCPKG_CXX_FLAGS "/Zc:__cplusplus /permissive /utf-8" CACHE STRING "")
set(VCPKG_CXX_FLAGS "/std:c++latest /Zc:__cplusplus /permissive /utf-8" CACHE STRING "")
#set(VCPKG_CXX_FLAGS_DEBUG "${VCPKG_CXX_FLAGS_DEBUG} /fsanitize=address" CACHE STRING "")
#set(VCPKG_LINKER_FLAGS_DEBUG "${VCPKG_LINKER_FLAGS_DEBUG} /INCREMENTAL:NO" CACHE STRING "")
set(VCPKG_C_FLAGS "/utf-8" CACHE STRING "")
#set(VCPKG_C_FLAGS_DEBUG "${VCPKG_C_FLAGS_DEBUG} /fsanitize=address" CACHE STRING "")

# https://developer.nvidia.com/cuda-gpus
# GeForce RTX 30 Series (Ampere): 8.6
# GeForce RTX 40 Series (Ada Lovelace): 8.9
#set(VCPKG_CUDA_ARCHITECTURES "86;89")
#set(CMAKE_CUDA_ARCHITECTURES "86;89" CACHE STRING "") # FIXME
set(CMAKE_CUDA_ARCHITECTURES "86")
#set(CUDA_ARCHITECTURES "80")

set(CMAKE_CUDA_STANDARD 20) # c++20
