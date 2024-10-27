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
        winget uninstall --id $winget_visualstudio_id --silent --disable-interactivity
    }
    winget install --id $winget_visualstudio_id --override "--quiet --config ${vsconfig}" --silent --disable-interactivity --accept-source-agreements
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
#    winget install -e --id Cppcheck.Cppcheck --silent --disable-interactivity --location "${Env:ProgramFiles}\Cppcheck" --accept-source-agreements
#    if (-not(Get-Command cppcheck.exe -ErrorAction SilentlyContinue)) {
#        Write-Host "Setup path to Cppcheck..."
#        $cppcheck_path = "${Env:ProgramFiles}\Cppcheck"
#        $user_path = [System.Environment]::GetEnvironmentVariable("PATH", "User")
#        $cppcheck_path_exists = @($user_path -split ';') -contains $cppcheck_path
#        if ($Env:GITHUB_ACTIONS) {
#            Write-Host "Run on GitHub Actions"
#            $Env:PATH += ";${cppcheck_path}"
#            Set-Content -Path $Env:GITHUB_PATH $Env:PATH
#        } elseif (-not($cppcheck_path_exists)) {
#            $user_path += ";${cppcheck_path}"
#            [System.Environment]::SetEnvironmentVariable("PATH", $user_path, "User")
#            Write-Host "Restart vscode or powershell process to reflect the changes."
#        }
#    }
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
