$ErrorActionPreference = 'Stop'

if ($Env:ENVS_PS1_LOADED -eq "1") {
    return
}

chcp 65001
#chcp 932

if ($Env:VCINSTALLDIR -eq $null) {
    $vsdevcmd_script = (Join-Path "${Env:ProgramFiles(x86)}" 'Microsoft Visual Studio\2022\BuildTools\Common7\Tools\Launch-VsDevShell.ps1')
    if (Test-Path $vsdevcmd_script) {
        . $vsdevcmd_script
    }
}

$Env:VCPKG_ROOT = $PSScriptRoot + '\vendor\vcpkg'
$Env:PATH = "${Env:VCPKG_ROOT};${Env:PATH}"
if (-not(Test-Path "${Env:VCPKG_ROOT}\.git")) {
    git clone https://github.com/microsoft/vcpkg.git $Env:VCPKG_ROOT
    cd $Env:VCPKG_ROOT
    .\bootstrap-vcpkg.bat
    cd -
}

#$Env:VCPKG_TARGET_TRIPLET = "x64-windows"
#$Env:VCPKG_TARGET_TRIPLET = "x86-windows"

#$Env:VCPKG_OVERLAY_TRIPLETS = ($PSScriptRoot + '..\triplets')
#vcpkg install --triplet $Env:VCPKG_TARGET_TRIPLET
vcpkg install

$vcpkg_tools_path = (vcpkg env --tools "echo %PATH%")
# 先頭に追加するとarchがズレるので末尾に追加
$Env:PATH = "${Env:PATH};${vcpkg_tools_path}"

$Env:ENVS_PS1_LOADED = "1"
