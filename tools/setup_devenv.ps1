Param(
    [switch]$clean
)

Import-Module $PSScriptRoot\choco_utils.psm1
Import-Module $PSScriptRoot\vs_utils.psm1

Function InstallVisualStudio([boolean]$clean) {
    if (-not($clean) -and (PathToVisualStudio -with_vsconfig)) {
        Write-Host "VisualStudio is already installed."
        return
    }
    $installed_path = (PathToVisualStudio)
    if ($installed_path -and -not($clean)) {
        ModifyVSWithConfig $installed_path
    } else {
        # The Community edition is compatible with The Enterprise edition. https://github.com/actions/runner-images/blob/main/images/windows/Windows2022-Readme.md#visual-studio-enterprise-2022
        $winget_visualstudio_id = "Microsoft.VisualStudio.2022.Community"
        $vsconfig = ".vsconfig"
        if ($clean) {
            winget uninstall --id $winget_visualstudio_id -e --silent --disable-interactivity
        }
        winget install --id $winget_visualstudio_id -e --override "--quiet --config ${vsconfig}" --silent --disable-interactivity --accept-source-agreements
    }
    foreach ($_ in 1..300) {
        Write-Host "Waiting for VS Installer... ${_}";
        Start-Sleep 3;
        $exists = PathToVisualStudio -with_vsconfig
        if ($exists) {
            Write-Host "Installed VisualStudio at ${exists}"
            return
        }
    }
    throw "VisualStudio not found."
}

Function InstallOthers([boolean]$clean) {
    $cuda_version = "12.6"
    if ($clean) {
        winget uninstall -e --id Nvidia.CUDA -v $cuda_version --silent --disable-interactivity
    }
    # https://docs.nvidia.com/cuda/cuda-installation-guide-microsoft-windows/#id3
    $sub_packages = @(
        "nvcc_${cuda_version}"
        , "nvml_dev_${cuda_version}"
        , "nvrtc_${cuda_version}"
        , "nvrtc_dev_${cuda_version}"
        , "npp_${cuda_version}"
        , "npp_dev_${cuda_version}"
        , "nvjpeg_${cuda_version}"
        , "nvjpeg_dev_${cuda_version}"
        , "cudart_${cuda_version}"
        , "cublas_${cuda_version}"
        , "cublas_dev_${cuda_version}"
        , "cufft_${cuda_version}"
        , "cufft_dev_${cuda_version}"
        , "curand_${cuda_version}"
        , "curand_dev_${cuda_version}"
        , "cusolver_${cuda_version}"
        , "cusolver_dev_${cuda_version}"
        , "cusparse_${cuda_version}"
        , "cusparse_dev_${cuda_version}"
    )
    winget install -e --id Nvidia.CUDA -v $cuda_version --silent --disable-interactivity --accept-source-agreements `
        --override ("-s {0} -n" -F ($sub_packages -join " "))
    $installed_path = "${Env:ProgramFiles}\NVIDIA GPU Computing Toolkit\CUDA\v${cuda_version}"
    if (-not(Test-Path $installed_path)) {
        throw "CUDA not found."
    }
    $Env:CUDA_PATH = $installed_path
    [System.Environment]::SetEnvironmentVariable("CUDA_PATH", $Env:CUDA_PATH, [System.EnvironmentVariableTarget]::User)
    if (($Env:PATH -split ";") -notcontains "${Env:CUDA_PATH}\bin") {
        $Env:PATH = "${Env:CUDA_PATH}\bin;${Env:PATH}"
        [System.Environment]::SetEnvironmentVariable("PATH", $Env:PATH, [System.EnvironmentVariableTarget]::User)
    }
}

Function Main([boolean]$clean) {
    pushd $PSScriptRoot > $null

    SetupPathToVSInstaller
    InstallVisualStudio $clean
    InstallOthers $clean
    popd > $null
}

$ErrorActionPreference = "Stop"
Set-PSDebug -Trace 1
Main -clean $clean.IsPresent
