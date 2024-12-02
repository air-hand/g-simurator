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
cmake_policy(SET CMP0155 NEW)


project(simurator)

find_package(directxtk CONFIG REQUIRED)
find_package(plog CONFIG REQUIRED)
find_package(OpenCV CONFIG REQUIRED)
find_package(protobuf CONFIG REQUIRED)
find_package(boost_program_options CONFIG REQUIRED)
find_package(Tesseract CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 23) # c++latest
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG")
set(CXX_FLAGS_SHARED
    "/Zc:__cplusplus"
    "/utf-8"
    "/EHsc"
    "/permissive-"
    "/analyze" # https://learn.microsoft.com/en-us/cpp/code-quality/code-analysis-for-c-cpp-overview?view=msvc-170#command-line-support
    "/analyze:external-"
    # disable warnings
    "/wd4267"
    "/wd4365"
    "/wd4686"
    "/wd4820"
    "/wd5050"
    "/wd5244"
    "/wd6326"
    "/external:anglebrackets"
    "/external:W0"
)

add_library(${{PROJECT_NAME}}-std STATIC)
{0}
target_compile_options(${{PROJECT_NAME}}-std
    PRIVATE
    ${{CXX_FLAGS_SHARED}}
    /Wall
    /WX
)

add_library(${{PROJECT_NAME}}-proto STATIC)
{1}

set_target_properties(${{PROJECT_NAME}}-proto
    PROPERTIES
    CXX_VISIBILITY_PRESET hidden
)

target_compile_options(${{PROJECT_NAME}}-proto
    PRIVATE
    ${{CXX_FLAGS_SHARED}}
    /W0
    /analyze-
)

target_include_directories(${{PROJECT_NAME}}-proto
    PRIVATE
)

target_link_libraries(${{PROJECT_NAME}}-proto
    PRIVATE
    protobuf::libprotobuf
)

#add_library(${{PROJECT_NAME}}-utils SHARED)
add_library(${{PROJECT_NAME}}-utils STATIC)
{2}

set_target_properties(${{PROJECT_NAME}}-utils
    PROPERTIES
    CXX_VISIBILITY_PRESET hidden
)

target_compile_options(${{PROJECT_NAME}}-utils
    PRIVATE
    ${{CXX_FLAGS_SHARED}}
    /Wall
    /WX
)
target_compile_definitions(${{PROJECT_NAME}}-utils
    PRIVATE
)
target_precompile_headers(${{PROJECT_NAME}}-utils
    PRIVATE
    ${{CMAKE_CURRENT_SOURCE_DIR}}/src/std/pch.hpp
)

target_include_directories(${{PROJECT_NAME}}-utils
    PRIVATE
    ${{OpenCV_INCLUDE_DIRS}}
    ${{CMAKE_CURRENT_SOURCE_DIR}}/src
)

target_link_libraries(${{PROJECT_NAME}}-utils
    PRIVATE
    d3d11.lib
    dxgi.lib
    uuid.lib
    Microsoft::DirectXTK
    ${{OpenCV_LIBS}}
    plog::plog
    Tesseract::libtesseract
    ${{PROJECT_NAME}}-std
)
target_link_options(${{PROJECT_NAME}}-utils
    PRIVATE
    /WX
)

add_executable(${{PROJECT_NAME}})
{3}

target_compile_options(${{PROJECT_NAME}}
    PRIVATE
    ${{CXX_FLAGS_SHARED}}
    /Wall
    /WX
    /Qspectre
    /wd5045
)
target_precompile_headers(${{PROJECT_NAME}}
    PRIVATE
    ${{CMAKE_CURRENT_SOURCE_DIR}}/src/std/pch.hpp
)

target_include_directories(${{PROJECT_NAME}}
    PRIVATE
    ${{OpenCV_INCLUDE_DIRS}}
    ${{CMAKE_CURRENT_SOURCE_DIR}}/src
    ${{CMAKE_CURRENT_SOURCE_DIR}}/src/main
)

target_link_libraries(${{PROJECT_NAME}}
    PRIVATE
    Microsoft::DirectXTK
    Boost::program_options
    ${{PROJECT_NAME}}-std
    ${{PROJECT_NAME}}-proto
    ${{PROJECT_NAME}}-utils
)
target_link_options(${{PROJECT_NAME}}
    PRIVATE
    /WX
)

# tests
enable_testing()

find_package(GTest CONFIG REQUIRED)
add_executable(${{PROJECT_NAME}}-test)
{4}

target_compile_options(${{PROJECT_NAME}}-test
    PRIVATE
    ${{CXX_FLAGS_SHARED}}
    /Wall
    /WX
)
target_precompile_headers(${{PROJECT_NAME}}-test
    PRIVATE
    ${{CMAKE_CURRENT_SOURCE_DIR}}/src/std/pch.hpp
)

target_include_directories(${{PROJECT_NAME}}-test
    PRIVATE
    ${{CMAKE_CURRENT_SOURCE_DIR}}/src
    ${{CMAKE_CURRENT_SOURCE_DIR}}/src/tests
)

target_link_libraries(${{PROJECT_NAME}}-test
    PRIVATE
    GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main
    ${{PROJECT_NAME}}-std
    ${{PROJECT_NAME}}-utils
)
target_link_options(${{PROJECT_NAME}}-test
    PRIVATE
    /WX
)

add_test(NAME test COMMAND ${{PROJECT_NAME}}-test)

#gtest_discover_tests(${{PROJECT_NAME}}-test)

# tessdata
set(FILES_TO_DOWNLOAD
    "https://github.com/tesseract-ocr/tessdata/raw/main/eng.traineddata|$ENV{{TESSDATA_PREFIX}}/eng.traineddata|DAA0C97D651C19FBA3B25E81317CD697E9908C8208090C94C3905381C23FC047"
    "https://github.com/tesseract-ocr/tessdata/raw/main/jpn.traineddata|$ENV{{TESSDATA_PREFIX}}/jpn.traineddata|6F416B902D129D8CC28E99C33244034B1CF52549E8560F6320B06D317852159A"
)

foreach(files ${{FILES_TO_DOWNLOAD}})
    string(REPLACE "|" ";" files_list ${{files}})
    list(GET files_list 0 URL)
    list(GET files_list 1 DEST)
    list(GET files_list 2 SHA256_HASH)

    file(DOWNLOAD
        ${{URL}}
        ${{DEST}}
        EXPECTED_HASH SHA256=${{SHA256_HASH}}
        SHOW_PROGRESS
    )
endforeach()
'@ -F (SourceSubDirectories 'std'), (SourceSubDirectories 'proto'), (SourceSubDirectories 'utils'), (SourceSubDirectories 'main'), (SourceSubDirectories 'tests'))

Set-Content -Path CMakeLists.txt -Value $content
