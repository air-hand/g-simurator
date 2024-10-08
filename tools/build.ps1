$ErrorActionPreference = "Stop"

cd $PSScriptRoot
cd ..

#protoc -I./src/proto --cpp_out=./src/proto/gen ./src/proto/*.proto

Get-ChildItem ./ -Include "CMake*.ps1" -Exclude @("./build", "./tools") -Recurse | Sort-Object | % {
    Write-Host "Running $_"
    pwsh $_
}

Write-Host $Env:PATH
cmake --preset=vcpkg -DCMAKE_BUILD_TYPE=Debug --fresh
cmake --build .\build

if ($? -ne $true) {
    throw "Build failed."
}
# https://discourse.cmake.org/t/ctest-scripting-cmake-presets/9610/2
#ctest --build .\build --preset=test -C Debug
