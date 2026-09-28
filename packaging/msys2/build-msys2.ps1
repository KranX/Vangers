#Requires -Version 5.1
<#
.SYNOPSIS
    Build Vangers with MSYS2.

.DESCRIPTION
    Configures, builds and tests Vangers inside the MSYS2 environment. The
    dependencies must already be installed by install-msys2.ps1.

.PARAMETER Msys2Root
    MSYS2 installation directory. Default: C:\msys64

.PARAMETER BuildType
    CMake build type. Default: RelWithDebInfo

.PARAMETER BuildDir
    Build directory, repo-relative or absolute. Default: build

.PARAMETER SkipTests
    Do not run ctest after building.

.EXAMPLE
    .\build-msys2.ps1
.EXAMPLE
    .\build-msys2.ps1 -SkipTests
#>
[CmdletBinding()]
param(
    [string]$Msys2Root = 'C:\msys64',
    [string]$BuildType = 'RelWithDebInfo',
    [string]$BuildDir = 'build',
    [switch]$SkipTests
)

$ErrorActionPreference = 'Stop'

$bash = Join-Path $Msys2Root 'usr\bin\bash.exe'
if (-not (Test-Path $bash)) {
    throw "MSYS2 not found at '$Msys2Root'. Run install-msys2.ps1 first, or set -Msys2Root."
}

# --- run the build inside MSYS2 ---------------------------------------------
$env:MSYSTEM = 'UCRT64'
$env:BUILD_TYPE = $BuildType
$env:BUILD_DIR = $BuildDir
$env:SKIP_TESTS = if ($SkipTests) { '1' } else { '0' }

$script = (Join-Path $PSScriptRoot 'build-msys2.sh') -replace '\\', '/'
Write-Host "Running '$script' in MSYS2 ($env:MSYSTEM)..."
& $bash --login $script
if ($LASTEXITCODE -ne 0) {
    throw "build-msys2.sh failed with exit code $LASTEXITCODE."
}
