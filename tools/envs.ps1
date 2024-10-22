$ErrorActionPreference = 'Stop'

if ($Env:ENVS_PS1_LOADED -eq "1") {
    return
}

chcp 65001

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
    git clone https://github.com/microsoft/vcpkg.git $Env:VCPKG_ROOT
    cd $Env:VCPKG_ROOT
    .\bootstrap-vcpkg.bat
    cd -
}

push $PSScriptRoot\.. > $null

$Env:VCPKG_TARGET_TRIPLET = "x64-windows"
vcpkg install --triplet $Env:VCPKG_TARGET_TRIPLET

$vcpkg_tools_path = (vcpkg env --tools "echo %PATH%")
# avoid arch mismatch by adding to the end
$Env:PATH = "${Env:PATH};${vcpkg_tools_path}"

$Env:ENVS_PS1_LOADED = "1"

popd > $null
