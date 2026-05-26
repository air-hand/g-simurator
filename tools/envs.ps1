$ErrorActionPreference = 'Stop'

chcp 65001

if ($Env:ENVS_PS1_LOADED -eq "1") {
    return
}

cd $PSScriptRoot


Import-Module $PSScriptRoot\vs_utils.psm1
SetupPathToVSInstaller
$visual_studio = PathToVisualStudio
if (-not($visual_studio)) {
    throw "Visual Studio not found."
}
if ($Env:VCINSTALLDIR -eq $null) {
    $vsdevcmd_script = (Join-Path "${visual_studio}" 'Common7\Tools\Launch-VsDevShell.ps1')
    if (-not(Test-Path $vsdevcmd_script)) {
        throw "Launch-VsDevShell.ps1 not found."
    }
    . $vsdevcmd_script -Arch amd64 -VsInstallationPath $visual_studio
}

$Env:VCPKG_VISUAL_STUDIO_PATH = $visual_studio
# need to ENV{VCPKG_ROOT} be used by vcpkg custom toolchain and so on
$Env:VCPKG_KEEP_ENV_VARS = "VCPKG_ROOT"

$Env:VCPKG_ROOT = $PSScriptRoot + '\vendor\vcpkg'
$Env:PATH = "${Env:VCPKG_ROOT};${Env:PATH}"
if (-not(Test-Path "${Env:VCPKG_ROOT}\.git")) {
    git clone https://github.com/microsoft/vcpkg.git $Env:VCPKG_ROOT
}
pushd $Env:VCPKG_ROOT > $null
#git remote set-url origin https://github.com/air-hand/vcpkg.git
#git remote set-url origin https://github.com/microsoft/vcpkg.git
git fetch --all --tags
#git checkout 2026.04.27
# CUDA 13.2 fixed, not releases tag yet
git checkout 940f58770cee8e2011bfb4847fb2bd70057301a8
.\bootstrap-vcpkg.bat
popd > $null

cd $PSScriptRoot\..
$Env:VCPKG_TARGET_TRIPLET = "x64-windows"
if (-not($Env:VCPKG_BINARY_SOURCES)) {
    $Env:VCPKG_BINARY_SOURCES = "clear;files,D:\vcpkg-binary-cache,readwrite"
}

# https://learn.microsoft.com/en-us/vcpkg/consume/binary-caching-github-packages
# Configure NuGet for vcpkg binary caching in CI environment only
if ($Env:CI -and $Env:NUGET_FEED_URL -and $Env:NUGET_TOKEN) {
    $nuget_exe = (vcpkg fetch nuget | Out-String).Trim()
    if ($nuget_exe -and (Test-Path $nuget_exe)) {
        Write-Host "Configuring NuGet source for vcpkg binary caching..."
        & $nuget_exe sources add `
          -Source "$Env:NUGET_FEED_URL" `
          -StorePasswordInClearText `
          -Name "GitHubPackages" `
          -UserName "$Env:NUGET_USERNAME" `
          -Password "$Env:NUGET_TOKEN" `
          -NonInteractive 2>$null
        & $nuget_exe setapikey "$Env:NUGET_TOKEN" `
          -Source "$Env:NUGET_FEED_URL" `
          -NonInteractive 2>$null
        Write-Host "NuGet source configured successfully."
    }
}

vcpkg install --triplet $Env:VCPKG_TARGET_TRIPLET

$vcpkg_tools_path = (vcpkg env --tools "echo %PATH%")
$Env:PATH = "${vcpkg_tools_path};${Env:PATH}"

# CUDA
$cuda_bin_path = "${Env:CUDA_PATH}\bin"
if (($Env:PATH -split ";") -notcontains $cuda_bin_path) {
    $Env:PATH = "${cuda_bin_path};${Env:PATH}"
}

$Env:TESSDATA_PREFIX = (Resolve-Path $PSScriptRoot\..\tessdata)

$Env:ENVS_PS1_LOADED = "1"
