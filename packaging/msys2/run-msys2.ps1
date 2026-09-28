#Requires -Version 5.1
<#
.SYNOPSIS
    Copy the built Vangers binaries and their MSYS2 runtime DLLs into a game
    directory and (optionally) launch the game.

.EXAMPLE
    .\run-msys2.ps1 ..\..\vangers\bin
.EXAMPLE
    .\run-msys2.ps1 ..\..\vangers\bin -NoRun
#>
[CmdletBinding()]
param(
    [Parameter(Mandatory = $true, Position = 0)]
    [string]$GameDir,

    [switch]$NoRun,

    [string]$Msys2Root = 'C:\msys64',
    [string]$BuildDir = 'build'
)

$ErrorActionPreference = 'Stop'

function ConvertTo-MsysPath([string]$Path) {
    $full = [System.IO.Path]::GetFullPath($Path)
    if ($full -match '^([A-Za-z]):\\(.*)$') {
        $drive = $Matches[1].ToLowerInvariant()
        $rest = $Matches[2] -replace '\\', '/'
        return "/$drive/$rest"
    }
    return ($full -replace '\\', '/')
}

$bash = Join-Path $Msys2Root 'usr\bin\bash.exe'
if (-not (Test-Path $bash)) {
    throw "MSYS2 bash not found at '$bash'. Install MSYS2 or set -Msys2Root."
}

$gameDirFull = [System.IO.Path]::GetFullPath($GameDir)

$env:MSYSTEM = 'UCRT64'
$env:GAME_DIR = ConvertTo-MsysPath $gameDirFull
$env:BUILD_DIR = $BuildDir

$script = (Join-Path $PSScriptRoot 'run-msys2.sh') -replace '\\', '/'
Write-Host "Running '$script' in MSYS2 ($env:MSYSTEM)..."
& $bash --login $script
if ($LASTEXITCODE -ne 0) {
    throw "run-msys2.sh failed with exit code $LASTEXITCODE."
}

if ($NoRun) {
    Write-Host 'Skipping launch (-NoRun).'
    return
}

Write-Host "Launching vangers.exe (working directory '$gameDirFull')..."
Start-Process -FilePath (Join-Path $gameDirFull 'vangers.exe') -WorkingDirectory $gameDirFull
