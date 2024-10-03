Function SetupPathToVSInstaller() {
    $vswhere = @('Microsoft Visual Studio', 'Installer', 'vswhere.exe') | % { $p = ${Env:ProgramFiles(x86)}; } { $p = Join-Path $p $_ } { $p }

    If (-not(Test-Path $vswhere)) {
        throw "vswhere.exe not found."
    }

    $Env:PATH = ("{0};${Env:PATH}" -F (Split-Path $vswhere))
}

Function InstallBuildTools() {
    $buildtools_product_id = "Microsoft.VisualStudio.Product.BuildTools"
    $channel_id = "VisualStudio.17.Release"
    $expect_buildtools_name = "Visual Studio Build Tools 2022"
    $buildtools_name = (vswhere.exe -products $buildtools_product_id -property displayName)
    if ($expect_buildtools_name -cne $buildtools_name) {
        vs_installer.exe install --productId=$buildtools_product_id --channelId=$channel_id
#        throw ("BuildTools expect '{0}', but '{1}'" -F $expect_buildtools_name, $buildtools_name)
    }
    $vs_components = @(
        'Microsoft.VisualStudio.Component.VC.Tools.x86.x64'
        , 'Microsoft.VisualStudio.Component.VC.CMake.Project'
        , 'Microsoft.VisualStudio.Component.Windows11SDK.26100'
        , 'Microsoft.VisualStudio.Component.VC.ASAN'
        , 'Microsoft.VisualStudio.Component.TestTools.BuildTools'
    )
#    $vs_args = @("modify", "--quiet", "--productId=${buildtools_product_id}", "--channelId=${channel_id}")
    $vs_args = @("modify", "--quiet", "--config", "vsconfig.json")
    #$vs_args += ($vs_components | % { "--add=${_}" })
    winget install --id Microsoft.VisualStudio.2022.BuildTools --override "--quiet --config vsconfig.json" --silent --disable-interactivity --accept-source-agreements
#    Start-Process vs_installer.exe -ArgumentList $vs_args -Wait
    #Start-Process vs_buildtools.exe -ArgumentList $vs_args -Wait
#    $vs_components | % {
#        Start-Process vs_installer.exe -ArgumentList @("modify", "--quiet", "--productId=${buildtools_product_id}", "--channelId=${channel_id}", "--add=${_}") -Wait
#    }
    $r = (vswhere.exe -products $buildtools_product_id -property installationPath -requires ("{0}" -F ($vs_components -join " ")))
}

Function Main() {
    cd $PSScriptRoot

    SetupPathToVSInstaller
    InstallBuildTools
}

$ErrorActionPreference = "Stop"
Main
