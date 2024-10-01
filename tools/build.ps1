cd $PSScriptRoot

$Env:VCPKG_ROOT = Resolve-Path (".\vendor\vcpkg")
$Env:PATH = "${Env:VCPKG_ROOT};${Env:PATH}"

if (-not(Test-Path "${Env:VCPKG_ROOT}\.git")) {
    git clone https://github.com/microsoft/vcpkg.git $Env:VCPKG_ROOT
    cd $Env:VCPKG_ROOT
    .\bootstrap-vcpkg.bat
    cd -
}

cd ..

Get-ChildItem ./ -Include "CMake*.ps1" -Exclude @("./build", "./tools") -Recurse | Sort-Object | % {
    Write-Host "Running $_"
    pwsh $_
}

cmake --preset=vcpkg
cmake --build build
