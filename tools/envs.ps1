$ErrorActionPreference = 'Stop'

if ($Env:ENVS_PS1_LOADED -eq "1") {
    return
}

chcp 65001

cd $PSScriptRoot

if ($Env:VCINSTALLDIR -eq $null) {
    Import-Module $PSScriptRoot\vs_utils.psm1
    SetupPathToVSInstaller
    $visual_studio = PathToVisualStudio
    $vsdevcmd_script = (Join-Path "${visual_studio}" 'Common7\Tools\Launch-VsDevShell.ps1')
    if (-not(Test-Path $vsdevcmd_script)) {
        throw "Launch-VsDevShell.ps1 not found."
    }
    . $vsdevcmd_script -Arch amd64
}

$Env:VCPKG_ROOT = $PSScriptRoot + '\vendor\vcpkg'
$Env:PATH = "${Env:VCPKG_ROOT};${Env:PATH}"
if (-not(Test-Path "${Env:VCPKG_ROOT}\.git")) {
#    git clone https://github.com/microsoft/vcpkg.git $Env:VCPKG_ROOT
    git clone https://github.com/air-hand/vcpkg.git $Env:VCPKG_ROOT
}
pushd $Env:VCPKG_ROOT > $null
git switch opencv-cuda
git fetch
git reset --hard origin/opencv-cuda
.\bootstrap-vcpkg.bat
popd > $null

cd $PSScriptRoot\..
$Env:VCPKG_TARGET_TRIPLET = "x64-windows"
if (-not($Env:VCPKG_BINARY_SOURCES)) {
    $Env:VCPKG_BINARY_SOURCES = "clear;"
}
vcpkg install --triplet $Env:VCPKG_TARGET_TRIPLET

# test
Get-ChildItem .\vcpkg_installed\x64-windows\tools -Recurse
exit 1

$vcpkg_tools_path = (vcpkg env --tools "echo %PATH%")
$Env:PATH = "${vcpkg_tools_path};${Env:PATH}"

# CUDA
$cuda_bin_path = "${Env:CUDA_PATH}\bin"
if (($Env:PATH -split ";") -notcontains $cuda_bin_path) {
    $Env:PATH = "${cuda_bin_path};${Env:PATH}"
}

$Env:TESSDATA_PREFIX = (Resolve-Path $PSScriptRoot\..\tessdata)

$Env:ENVS_PS1_LOADED = "1"
