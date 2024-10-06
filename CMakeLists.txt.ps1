cd $PSScriptRoot

$add_subdirectory = (
    Get-ChildItem .\src -Include "CMakeLists.txt.ps1" -Recurse | Sort-Object | % {
        return ((Resolve-Path (Split-Path -Parent $_) -Relative) -replace '\\', '/')
    } | % {
        return ("add_subdirectory({0})" -F $_)
    } | Join-String -Separator "`r`n"
)

$content = (@'
cmake_minimum_required(VERSION 3.29)

project(simurator)

find_package(directxtk CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
#find_package(boost_log CONFIG REQUIRED)
find_package(plog CONFIG REQUIRED)

add_executable(simurator)
{0}

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
target_compile_features(simurator PRIVATE cxx_std_20)
target_compile_options(simurator PRIVATE "/Zc:__cplusplus")
target_compile_options(simurator PRIVATE "/utf-8") # utf-8 source and execution character set

target_link_libraries(simurator PRIVATE Microsoft::DirectXTK)
target_link_libraries(simurator PRIVATE fmt::fmt)
target_link_libraries(simurator PRIVATE plog::plog)
'@ -F $add_subdirectory)

Set-Content -Path CMakeLists.txt -Value $content
