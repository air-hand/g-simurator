Function SourceSubDirectories([string]$dir) {
    Get-ChildItem ".\src\${dir}" -Include "CMakeLists.txt.ps1" -Recurse | Sort-Object | % {
        return ((Resolve-Path (Split-Path -Parent $_) -Relative) -replace '\\', '/')
    } | % {
        return ("add_subdirectory({0})" -F $_)
    } | Join-String -Separator "`r`n"
}

cd $PSScriptRoot

$content = (@'
cmake_minimum_required(VERSION 3.29)

project(simurator)

find_package(directxtk CONFIG REQUIRED)
find_package(plog CONFIG REQUIRED)
find_package(OpenCV CONFIG REQUIRED)
find_package(protobuf CONFIG REQUIRED)
find_package(boost_program_options CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG")

#add_library(${{PROJECT_NAME}}-utils STATIC)
add_library(${{PROJECT_NAME}}-utils SHARED)
{0}

set_target_properties(${{PROJECT_NAME}}-utils
    PROPERTIES
    CXX_VISIBILITY_PRESET hidden
)

target_compile_options(${{PROJECT_NAME}}-utils
    PRIVATE
    "/Zc:__cplusplus"
    "/utf-8"
)
target_compile_definitions(${{PROJECT_NAME}}-utils
    PRIVATE
    UTILS_LIB_BUILD
)

target_include_directories(${{PROJECT_NAME}}-utils PRIVATE ${{OpenCV_INCLUDE_DIRS}})

target_link_libraries(${{PROJECT_NAME}}-utils
    PRIVATE
    ${{OpenCV_LIBS}}
    plog::plog
)

add_executable(${{PROJECT_NAME}})
{1}

target_compile_options(${{PROJECT_NAME}}
    PRIVATE
    "/Zc:__cplusplus"
    "/utf-8"
)

target_include_directories(${{PROJECT_NAME}}
    PRIVATE
    ${{OpenCV_INCLUDE_DIRS}}
)

target_link_libraries(${{PROJECT_NAME}}
    PRIVATE
    Microsoft::DirectXTK
    protobuf::libprotobuf
    Boost::program_options
    ${{PROJECT_NAME}}-utils
)

# tests
enable_testing()

find_package(GTest CONFIG REQUIRED)
add_executable(${{PROJECT_NAME}}-test)
{2}

target_compile_options(${{PROJECT_NAME}}-test
    PRIVATE
    "/Zc:__cplusplus"
    "/utf-8"
)

target_include_directories(${{PROJECT_NAME}}-test PRIVATE ${{CMAKE_CURRENT_SOURCE_DIR}})

target_link_libraries(${{PROJECT_NAME}}-test
    PRIVATE
    GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main
    ${{PROJECT_NAME}}-utils
)

add_test(NAME test COMMAND ${{PROJECT_NAME}}-test)

#gtest_discover_tests(${{PROJECT_NAME}}-test)

# Lint
find_program(CPPCHECK_EXECUTABLE NAMES cppcheck)
if(CPPCHECK_EXECUTABLE)
    add_custom_target(
        cppcheck
        COMMAND ${{CPPCHECK_EXECUTABLE}}
        --enable=all
        --quiet
        --language=c++
        --std=c++20
        --suppress=missingInclude
        --force
        --error-exitcode=1
        ${{CMAKE_CURRENT_SOURCE_DIR}}/src
    )
    add_dependencies(${{PROJECT_NAME}} cppcheck)
    add_dependencies(${{PROJECT_NAME}}-utils cppcheck)
    add_dependencies(${{PROJECT_NAME}}-test cppcheck)
else()
    message(WARNING "cppcheck not found")
endif()

'@ -F (SourceSubDirectories 'utils'), (SourceSubDirectories 'main'), (SourceSubDirectories 'tests'))

Set-Content -Path CMakeLists.txt -Value $content
