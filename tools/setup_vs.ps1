﻿Function SetupPathToVSInstaller() {
    $vswhere = @('Microsoft Visual Studio', 'Installer', 'vswhere.exe') | % { $p = ${Env:ProgramFiles(x86)}; } { $p = Join-Path $p $_ } { $p }

    If (-not(Test-Path $vswhere)) {
        throw "vswhere.exe not found."
    }

    $Env:PATH = ("{0};${Env:PATH}" -F (Split-Path $vswhere))
}

Function InstallBuildTools([switch]$clean) {
    $winget_build_tools_id = "Microsoft.VisualStudio.2022.BuildTools"
    $buildtools_product_id = "Microsoft.VisualStudio.Product.BuildTools"
    $vsconfig = ".vsconfig"
    if ($clean) {
        winget uninstall --id $winget_build_tools_id --silent --disable-interactivity
    }
    winget install --id $winget_build_tools_id --override "--quiet --config ${vsconfig}" --silent --disable-interactivity --accept-source-agreements

    $components = (Get-Content -Raw $vsconfig | ConvertFrom-Json).components
    Write-Host "Requires: ${components}"

    $requires = ($components | % { "-requires {0}" -F $_ })
    $vswhere_cmd = (
        "vswhere.exe -products ${buildtools_product_id} -property installationPath ${requires}"
    )
    1..300 | % {
        Write-Host "Waiting for VS Installer... ${_}";
        Start-Sleep 3;
        $exists = ($vswhere_cmd | Invoke-Expression)
        if ($exists) {
            Write-Host "Installed VS BuildTools at ${exists}"
            break
        }
    }
    throw "VS BuildTools not found."
}

Function Main() {
    cd $PSScriptRoot

    SetupPathToVSInstaller
    InstallBuildTools
}

$ErrorActionPreference = "Stop"
Main
