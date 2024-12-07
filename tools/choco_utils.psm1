Function SetupChocolatey() {
    $chodo = Get-Command choco
    if ($choco) {
        return
    }

    $Env:ChocolateyInstall = "${Env:ProgramData}\chocoportable"
    iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))
    $Env:PATH = "${Env:ChocolateyInstall}\bin;${Env:PATH}"
}

Export-ModuleMember -Function *
