include("$ENV{VCPKG_ROOT}/scripts/toolchains/windows.cmake")

set(CMAKE_CXX_STANDARD 23 CACHE STRING "")
set(CMAKE_CXX_STANDARD_REQUIRED ON CACHE BOOL "")

set(CMAKE_CUDA_STANDARD 20 CACHE STRING "")
# forward to all ports by vcpkg
# https://developer.nvidia.com/cuda-gpus
# GeForce RTX 30 Series (Ampere): 8.6
# GeForce RTX 40 Series (Ada Lovelace): 8.9
set(CMAKE_CUDA_ARCHITECTURES "86" CACHE STRING "")
#set(CMAKE_CUDA_ARCHITECTURES "86;89" CACHE STRING "")
