Function SetupPathToVSInstaller() {
    $vswhere = @('Microsoft Visual Studio', 'Installer', 'vswhere.exe') | % { $p = ${Env:ProgramFiles(x86)}; } { $p = Join-Path $p $_ } { $p }

    If (-not(Test-Path $vswhere)) {
        throw "vswhere.exe not found."
    }

    $Env:PATH = ("{0};${Env:PATH}" -F (Split-Path $vswhere))
}

Function CheckBuildToolsExists()
{
    $buildtools_product_id = "Microsoft.VisualStudio.Product.BuildTools"
    $vsconfig = ".vsconfig"
    $components = (Get-Content -Raw $vsconfig | ConvertFrom-Json).components
    $requires = ($components | % { "-requires {0}" -F $_ })
    $vswhere_cmd = (
#        "vswhere.exe -products ${buildtools_product_id} -property installationPath ${requires}"
#        "vswhere.exe -products * -version 17.0,18.0 -property installationPath ${requires}"
        "vswhere.exe -products * -version 17.0,18.0 -property installationPath"
    )
    $exists = ($vswhere_cmd | Invoke-Expression)
    return $exists
}

Function InstallBuildTools([switch]$clean) {
    $winget_build_tools_id = "Microsoft.VisualStudio.2022.BuildTools"
    $vsconfig = ".vsconfig"
    if ($clean) {
        winget uninstall --id $winget_build_tools_id --silent --disable-interactivity
    }
    winget install --id $winget_build_tools_id --override "--quiet --config ${vsconfig}" --silent --disable-interactivity --accept-source-agreements
    foreach ($_ in 1..300) {
        Write-Host "Waiting for VS Installer... ${_}";
        Start-Sleep 3;
        $exists = CheckBuildToolsExists
        if ($exists) {
            Write-Host "Installed VS BuildTools at ${exists}"
            return
        }
    }
    throw "VS BuildTools not found."
}

Function InstallOthers() {
    winget install -e --id Cppcheck.Cppcheck --silent --disable-interactivity --location "${Env:ProgramFiles}\Cppcheck"
    if (-not(Get-Command cppcheck.exe -ErrorAction SilentlyContinue)) {
        Write-Host "Setup path to Cppcheck..."
        $cppcheck_path = "${Env:ProgramFiles}\Cppcheck"
        $user_path = [System.Environment]::GetEnvironmentVariable("PATH", "User")
        $cppcheck_path_exists = @($user_path -split ';') -contains $cppcheck_path
        if (-not($cppcheck_path_exists)) {
            $user_path += ";${cppcheck_path}"
            [System.Environment]::SetEnvironmentVariable("PATH", $user_path, "User")
            Write-Host "Restart vscode or powershell process to reflect the changes."
        }
        if ($Env:GITHUB_ACTIONS) {
            Write-Host "Run on GitHub Actions"
            $Env:PATH += ";${cppcheck_path}"
            Set-Content -Path $Env:GITHUB_PATH $Env:PATH
        }
    }
}

Function Main() {
    pushd $PSScriptRoot > $null

    SetupPathToVSInstaller
    if (-not(CheckBuildToolsExists)) {
        InstallBuildTools
    }
    InstallOthers
    popd > $null
}

$ErrorActionPreference = "Stop"
Main
