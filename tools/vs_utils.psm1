Function SetupPathToVSInstaller() {
    $vswhere = Join-Path "${Env:ProgramFiles(x86)}" "Microsoft Visual Studio\Installer\vswhere.exe"

    If (-not(Test-Path $vswhere)) {
        throw "vswhere.exe not found."
    }

    $Env:PATH = ("{0};${Env:PATH}" -F (Split-Path $vswhere))
}

Function PathToVisualStudio()
{
    $product_id = "Microsoft.VisualStudio.Product.Community"
    $vsconfig = Join-Path $PSScriptRoot ".vsconfig"
    $components = (Get-Content -Raw $vsconfig | ConvertFrom-Json).components
    $requires = ($components | % { "-requires {0}" -F $_ })
    $vswhere_cmd = (
#        "vswhere.exe -products ${product_id} -property installationPath ${requires}"
        "vswhere.exe -products * -version 17.0,18.0 -property installationPath ${requires}"
    )
    return ($vswhere_cmd | Invoke-Expression)
}

Export-ModuleMember -Function *
