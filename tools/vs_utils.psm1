# settings
# The Community edition is compatible with The Enterprise edition. https://github.com/actions/runner-images/blob/main/images/windows/Windows2022-Readme.md#visual-studio-enterprise-2022
# VS 2026 Community -> Microsoft.VisualStudio.Community
Set-Variable -Name VS_ID -Value "Microsoft.VisualStudio.Community" -Option Constant -Scope Script
Set-Variable -Name VS_VERSION_RANGE -Value "18.0,19.0" -Option Constant -Scope Script
Set-Variable -Name VS_INSIDERS -Value ($script:VS_ID -like "*Insiders*") -Option Constant -Scope Script
Set-Variable -Name MSVC_PREVIEW -Value $false -Option Constant -Scope Script

Function WingetVisualStudioID() {
    return $script:VS_ID
}

Function SetupPathToVSInstaller()
{
    $vswhere = Join-Path "${Env:ProgramFiles(x86)}" "Microsoft Visual Studio\Installer\vswhere.exe"

    If (-not(Test-Path $vswhere))
    {
        throw "vswhere.exe not found."
    }

    $Env:PATH = ("{0};${Env:PATH}" -F (Split-Path $vswhere))
}

Function PathToVisualStudio([switch]$with_vsconfig)
{
    $version = "-version ${script:VS_VERSION_RANGE}"
    if ($script:VS_INSIDERS) {
        $version = "${version} -prerelease"
    }
    $vswhere_cmd = (
        "vswhere.exe -products * ${version} -format json -utf8"
    )
    if ($with_vsconfig)
    {
        $vsconfig = Join-Path $PSScriptRoot ".vsconfig"
        $components = (Get-Content -Raw $vsconfig | ConvertFrom-Json).components
        $requires = ($components | % { "-requires {0}" -F $_ })
        $vswhere_cmd = "${vswhere_cmd} ${requires}"
    }
    $result = ($vswhere_cmd | Invoke-Expression | ConvertFrom-Json)
    return ($result | ? { $_.isPrerelease -eq $script:VS_INSIDERS } | Select-Object -First 1 -ExpandProperty installationPath)
}

Function ModifyVSWithConfig([string]$installed_path)
{
    $vsconfig = Join-Path $PSScriptRoot ".vsconfig"
    $vs_installer = Get-Command "vs_installer.exe"
    $vs_installer_args = (@(
            "modify",
            "--installPath", "`"${installed_path}`"",
            "--config", "`"${vsconfig}`"",
            "--quiet", "--norestart", "--force", "--downloadThenInstall"
        ))

    $cmd = ("{0} {1}" -F $vs_installer.Name, ($vs_installer_args -join " "))

    ($cmd | Invoke-Expression)
}

# https://devblogs.microsoft.com/cppblog/microsoft-c-msvc-build-tools-v14-51-preview-released-how-to-opt-in/#command-line-builds-using-powershell
function Import-VcVars64Preview([string]$visual_studio) {
    if (-not($script:MSVC_PREVIEW)) {
        Write-Warning "No import to preview vars."
        return
    }
    $vcvars64 = Join-Path $visual_studio 'VC\Auxiliary\Build\vcvars64.bat'
    if (-not(Test-Path $vcvars64)) {
        throw "vcvars64.bat not found."
    }

    $env_lines = & cmd.exe /c "`"$vcvars64`" -vcvars_ver=Preview && set"
    if ($LASTEXITCODE -ne 0) {
        throw "vcvars64.bat failed."
    }

    foreach ($line in $env_lines) {
        $index = $line.IndexOf('=')
        if ($index -le 0) {
            continue
        }
        $name = $line.Substring(0, $index)
        $value = $line.Substring($index + 1)
        [System.Environment]::SetEnvironmentVariable($name, $value, [System.EnvironmentVariableTarget]::Process)
    }
}

Export-ModuleMember -Function *
