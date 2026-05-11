include("${VCPKG_ROOT_DIR}/triplets/x64-windows.cmake")

# /permissive for protobuf.
#set(CMAKE_CXX_STANDARD 23)
#set(VCPKG_CXX_FLAGS "/std:c++23preview /Zc:__cplusplus /permissive /utf-8" CACHE STRING "")
#set(VCPKG_C_FLAGS "/utf-8" CACHE STRING "")

# https://developer.nvidia.com/cuda-gpus
# GeForce RTX 30 Series (Ampere): 8.6
# GeForce RTX 40 Series (Ada Lovelace): 8.9
set(CUDA_ARCHITECTURES "86")
