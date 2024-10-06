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
find_package(plog CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

add_executable(${{PROJECT_NAME}})
{0}

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG")
target_compile_features(${{PROJECT_NAME}} PRIVATE cxx_std_20)
target_compile_options(${{PROJECT_NAME}} PRIVATE "/Zc:__cplusplus")
target_compile_options(${{PROJECT_NAME}} PRIVATE "/utf-8") # utf-8 source and execution character set

target_link_libraries(${{PROJECT_NAME}} PRIVATE Microsoft::DirectXTK)
target_link_libraries(${{PROJECT_NAME}} PRIVATE plog::plog)
target_link_libraries(${{PROJECT_NAME}} PRIVATE nlohmann_json nlohmann_json::nlohmann_json)

# tests
enable_testing()

find_package(GTest CONFIG REQUIRED)
add_executable(${{PROJECT_NAME}}-test)
# FIXME: main.cppをテスト対象から外しつつcppを含めるか、libにしてlinkできるようにするか
add_subdirectory(./tests)

target_link_libraries(${{PROJECT_NAME}}-test GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
target_include_directories(${{PROJECT_NAME}}-test PRIVATE ${{CMAKE_CURRENT_SOURCE_DIR}})

add_test(NAME test COMMAND ${{PROJECT_NAME}}-test)

#gtest_discover_tests(${{PROJECT_NAME}}-test)

'@ -F $add_subdirectory)

Set-Content -Path CMakeLists.txt -Value $content
