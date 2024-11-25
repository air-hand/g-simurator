Param(
    [switch]$clean
)

Import-Module $PSScriptRoot\vs_utils.psm1

Function InstallVisualStudio([boolean]$clean) {
    if (-not($clean) -and (PathToVisualStudio)) {
        Write-Host "VisualStudio is already installed."
        return
    }
    # The Community edition is compatible with The Enterprise edition. https://github.com/actions/runner-images/blob/main/images/windows/Windows2022-Readme.md#visual-studio-enterprise-2022
    $winget_visualstudio_id = "Microsoft.VisualStudio.2022.Community"
    $vsconfig = ".vsconfig"
    if ($clean) {
        winget uninstall --id $winget_visualstudio_id -e --silent --disable-interactivity
    }
    winget install --id $winget_visualstudio_id -e --override "--quiet --config ${vsconfig}" --silent --disable-interactivity --accept-source-agreements
    foreach ($_ in 1..300) {
        Write-Host "Waiting for VS Installer... ${_}";
        Start-Sleep 3;
        $exists = PathToVisualStudio
        if ($exists) {
            Write-Host "Installed VisualStudio at ${exists}"
            return
        }
    }
    throw "VisualStudio not found."
}

Function InstallOthers() {
    winget install -e --id Nvidia.CUDA -v "12.6" --silent --disable-interactivity --accept-source-agreements `
        --override "-s nvcc_12.6 npp_12.6"
    $installed_path = "${Env:ProgramFiles}\NVIDIA GPU Computing Toolkit\CUDA\v12.6"
    Get-ChildItem $installed_path
    (Get-ChildItem Env: | % { "{0}: {1}" -F ($_.Key, $_.Value) })
    $Env:CUDA_PATH = $installed_path
}

Function Main([boolean]$clean) {
    pushd $PSScriptRoot > $null

    SetupPathToVSInstaller
    InstallVisualStudio $clean
    InstallOthers
    popd > $null
}

$ErrorActionPreference = "Stop"
Main -clean $clean.IsPresent
