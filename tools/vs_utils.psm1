Function SetupPathToVSInstaller() {
    $vswhere = Join-Path "${Env:ProgramFiles(x86)}" "Microsoft Visual Studio\Installer\vswhere.exe"

    If (-not(Test-Path $vswhere)) {
        throw "vswhere.exe not found."
    }

    $Env:PATH = ("{0};${Env:PATH}" -F (Split-Path $vswhere))
}

Function PathToVisualStudio([switch]$with_vsconfig)
{
    $vswhere_cmd = (
        "vswhere.exe -products * -version 17.0,18.0 -property installationPath"
    )
    if ($with_vsconfig) {
        $vsconfig = Join-Path $PSScriptRoot ".vsconfig"
        $components = (Get-Content -Raw $vsconfig | ConvertFrom-Json).components
        $requires = ($components | % { "-requires {0}" -F $_ })
        $vswhere_cmd = "${vswhere_cmd} ${requires}"
    }
    return ($vswhere_cmd | Invoke-Expression)
}

Function ModifyVSWithConfig([string]$installed_path)
{
    $vsconfig = Join-Path $PSScriptRoot ".vsconfig"
    #$vs_installer = Get-Command "vs_installer.exe"
    $vs_installer = Get-Command "setup.exe"
    $vs_installer_args = (@(
        "modify",
        "--installPath", "`"${installed_path}`"",
        "--config", "`"${vsconfig}`"",
        "--quiet", "--norestart", "--nocache", "--force", "--downloadThenInstall"
    ) -join " ")
    $isCI = $env:CI -eq 'true'
    if (-not($IsCI)) {
        Write-Host $vs_installer_args
        $proc = Start-Process -FilePath $vs_installer.Path -ArgumentList $vs_installer_args -Verb RunAs -Wait -PassThru
    } else {
        $proc = Start-Process -FilePath $vs_installer.Path -ArgumentList $vs_installer_args -Wait -PassThru
    }
    switch ($proc.ExitCode) {
        0 { return }
        default {
            throw ("VS modify failed. exit={0}" -F $proc.ExitCode)
        }
    }
}

Export-ModuleMember -Function *
