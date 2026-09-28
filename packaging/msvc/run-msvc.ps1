#Requires -Version 5.1
<#
.SYNOPSIS
    Copy the built game and its runtime DLLs into a game directory, then run it.

.EXAMPLE
    .\run-msvc.ps1 ..\..\vangers\bin

.EXAMPLE
    .\run-msvc.ps1 ..\..\vangers\bin -NoRun

.EXAMPLE
    .\run-msvc.ps1 ..\..\vangers\bin -BuildType Debug
#>
[CmdletBinding()]
param(
    [Parameter(Mandatory = $true, Position = 0)]
    [string]$GameDir,

    [switch]$NoRun,

    [ValidateSet('Release', 'RelWithDebInfo', 'MinSizeRel', 'Debug')]
    [string]$BuildType = 'RelWithDebInfo',

    [string]$VcpkgRoot = $env:VCPKG_ROOT,
    [string]$ClunkRoot,
    [string]$FfmpegRoot,
    [string]$BuildDir
)

$ErrorActionPreference = 'Stop'

$repo = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path

$isDebug = ($BuildType -eq 'Debug')
$configSuffix = if ($isDebug) { '-debug' } else { '' }

if (-not $VcpkgRoot)  { $VcpkgRoot  = Join-Path $env:USERPROFILE 'vcpkg' }
if (-not $ClunkRoot)  { $ClunkRoot  = Join-Path $repo "external\clunk-install$configSuffix" }
if (-not $FfmpegRoot) { $FfmpegRoot = Join-Path $repo 'external\ffmpeg' }
if (-not $BuildDir)   { $BuildDir   = Join-Path $repo "build-msvc$configSuffix" }

$vangersExe = Join-Path $BuildDir 'src\vangers.exe'
if (-not (Test-Path $vangersExe)) {
    throw "Built game not found at '$vangersExe'. Run build-msvc.ps1 first, or set -BuildDir."
}

$GameDir = [System.IO.Path]::GetFullPath($GameDir)
New-Item -ItemType Directory -Force -Path $GameDir | Out-Null

Write-Host "Copying game to '$GameDir'..."

# --- executables ------------------------------------------------------------
Copy-Item $vangersExe $GameDir -Force
foreach ($exe in @(
        (Join-Path $BuildDir 'surmap\surmap.exe'),
        (Join-Path $BuildDir 'server\vangers_server.exe'))) {
    if (Test-Path $exe) { Copy-Item $exe $GameDir -Force }
}

# --- runtime DLLs -----------------------------------------------------------
# Vangers, clunk and every dependency must use the same configuration, so pick
# the bin/ tree that matches -BuildType (Debug ships separate DLLs).
$vcpkgBin = if ($isDebug) { 'vcpkg_installed\x64-windows\debug\bin' } else { 'vcpkg_installed\x64-windows\bin' }
$ffmpegBin = if ($isDebug) { 'debug\bin' } else { 'bin' }
$dllDirs = @(
    (Join-Path $repo $vcpkgBin),
    (Join-Path $ClunkRoot 'bin'),
    (Join-Path $FfmpegRoot $ffmpegBin)
)
foreach ($dir in $dllDirs) {
    if (Test-Path $dir) {
        Get-ChildItem -Path $dir -Filter *.dll -File |
            ForEach-Object { Copy-Item $_.FullName $GameDir -Force }
    }
}

Write-Host "Done."

if ($NoRun) {
    Write-Host 'Skipping launch (-NoRun).'
    return
}

Write-Host "Launching vangers.exe (working directory '$GameDir')..."
Start-Process -FilePath (Join-Path $GameDir 'vangers.exe') -WorkingDirectory $GameDir
