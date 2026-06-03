using namespace System.IO

Param(
    [string]$build_type="Debug"
)

$ErrorActionPreference = 'Stop'

. $PSScriptRoot\envs.ps1

cd $PSScriptRoot\..

$BUILD_DIR = ".\build\${build_type}"
$exe = "${BUILD_DIR}\simurator-test.exe"

if ($Env:TEST_DEBUG -eq "1") {
    $temp = Join-Path ([Path]::GetTempPath()) "simurator-test-$([guid]::NewGuid())"
    New-Item -Path $temp -ItemType Directory > $null
    Write-Host "Created tempdir : ${temp}"
    try {
        $zip = Join-Path $temp "Dependencies.zip"
        Invoke-WebRequest -Uri https://github.com/lucasg/Dependencies/releases/download/v1.11.1/Dependencies_x64_Release.zip -OutFile $zip
        $expectedHash = "7D22DC00F1C09FD4415D48AD74D1CF801893E83B9A39944B0FCE6DEA7CEAEA99"
        $actualHash = (Get-FileHash -Algorithm SHA256 $zip).Hash.ToUpperInvariant()
        if ($actualHash -ne $expectedHash) {
            throw ".\Dependencies.zip hash mismatch. Expected ${expectedHash}, actual ${actualHash}"
        }
        Expand-Archive -Path $zip -DestinationPath $temp
        $dependencies_exe = "${temp}\Dependencies.exe"
        Write-Host "Check dependencies of ${exe}"
        & $dependencies_exe -chain -modules $exe
    } finally {
        Remove-Item $temp -Recurse
    }
}

& $exe --gtest_output=xml:test-result.xml

$exitCode = $LASTEXITCODE
Write-Host "ExitCode: $exitCode"

if ($exitCode -ne 0) {
    throw ("simurator-test.exe failed with exit code 0x{0:X8}" -f ($exitCode -band 0xffffffff))
}

## https://discourse.cmake.org/t/ctest-scripting-cmake-presets/9610/2
#$CMAKE_TEST_COMMAND = @(
#    "ctest"
#    , "--build"
#    , ".\build"
#    , "--preset=test"
#    , "-C"
#    , "Debug"
#)
#Invoke-Expression -Command ($CMAKE_TEST_COMMAND -join " ")
