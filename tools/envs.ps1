$ErrorActionPreference = 'Stop'

chcp 65001

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

#vcpkg install --triplet $Env:VCPKG_TARGET_TRIPLET

$vcpkg_tools_path = (vcpkg env --tools "echo %PATH%")
$Env:PATH = "${vcpkg_tools_path};${Env:PATH}"

Write-Host "envs.ps1 done."
