using namespace System.Diagnostics
Function Invoke-ProcessStreamed(
    [string]$FilePath,
    [string[]]$ArgumentList=@()
)
{
    try {
      $psi = [ProcessStartInfo]::new()
      $psi.FileName = $FilePath
      $psi.Arguments = ($ArgumentList -join ' ')
      $psi.RedirectStandardOutput = $true
      $psi.RedirectStandardError  = $true
      $psi.UseShellExecute = $false
      $psi.CreateNoWindow  = $true
      $p = [Process]::new();
      $p.StartInfo = $psi
      $p.EnableRaisingEvents = $true
      $script = {
        $data = $Event.SourceEventArgs.Data
        if (-not [string]::IsNullOrEmpty($data)) {
            [System.Console]::WriteLine($data)
        }
      }
      $stdoutEvent = Register-ObjectEvent -InputObject $p -EventName OutputDataReceived -Action $script
      $stderrEvent = Register-ObjectEvent -InputObject $p -EventName ErrorDataReceived -Action $script
      [void]$p.Start()
      $p.BeginOutputReadLine(); $p.BeginErrorReadLine();
      $p.WaitForExit()
      $p.CancelOutputRead(); $p.CancelErrorRead()
      Unregister-Event -SourceIdentifier $stdoutEvent
      Unregister-Event -SourceIdentifier $stderrEvent
      return $p.ExitCode
    } finally {
        if ($p) {
            $p.Dispose()
        }
    }
}

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
    ))
    $exit_code = Invoke-ProcessStreamed $vs_installer.Path $vs_installer_args
    switch ($exit_code) {
        0 { return }
        default {
            throw ("VS modify failed. exit={0}" -F $exit_code)
        }
    }
}

Export-ModuleMember -Function *
