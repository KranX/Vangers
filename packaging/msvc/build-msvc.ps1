#Requires -Version 5.1
<#
.SYNOPSIS
    Build Vangers with MSVC.

.DESCRIPTION
    Configures and builds Vangers into -BuildDir (default build-msvc[-debug]).
    The dependencies (vcpkg packages, FFmpeg and clunk) must already be
    installed by install-msvc.ps1 for the same -BuildType.

.PARAMETER VcpkgRoot
    vcpkg directory (the CMake toolchain lives there). Default: $env:VCPKG_ROOT

.PARAMETER ClunkRoot
    clunk install prefix. Default: <repo>\external\clunk-install[-debug]

.PARAMETER FfmpegRoot
    Prebuilt MSVC FFmpeg prefix. Default: <repo>\external\ffmpeg

.PARAMETER BuildDir
    Build directory. Default: <repo>\build-msvc[-debug]

.PARAMETER BuildType
    CMake build type. Default: RelWithDebInfo

.EXAMPLE
    .\build-msvc.ps1
.EXAMPLE
    .\build-msvc.ps1 -BuildType Debug
#>
[CmdletBinding()]
param(
    [string]$VcpkgRoot = $env:VCPKG_ROOT,
    [string]$ClunkRoot,
    [string]$FfmpegRoot,
    [string]$BuildDir,

    [ValidateSet('Release', 'RelWithDebInfo', 'MinSizeRel', 'Debug')]
    [string]$BuildType = 'RelWithDebInfo'
)

$ErrorActionPreference = 'Stop'

$repo = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
$external = Join-Path $repo 'external'

$isDebug = ($BuildType -eq 'Debug')
$configSuffix = if ($isDebug) { '-debug' } else { '' }

if (-not $VcpkgRoot)  { $VcpkgRoot  = Join-Path $env:USERPROFILE 'vcpkg' }
if (-not $ClunkRoot)  { $ClunkRoot  = Join-Path $external "clunk-install$configSuffix" }
if (-not $FfmpegRoot) { $FfmpegRoot = Join-Path $external 'ffmpeg' }
if (-not $BuildDir)   { $BuildDir   = Join-Path $repo "build-msvc$configSuffix" }

function Assert-Path([string]$Path, [string]$Message) {
    if (-not (Test-Path $Path)) { throw $Message }
}

Assert-Path (Join-Path $VcpkgRoot 'vcpkg.exe') `
    "vcpkg not found at '$VcpkgRoot'. Install it from https://learn.microsoft.com/vcpkg/get_started/get-started or set -VcpkgRoot."

# --- locate Visual Studio and import its environment ------------------------
$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
Assert-Path $vswhere "vswhere.exe not found. Install Visual Studio 2022+ with the 'Desktop development with C++' workload."

$vsInstall = (& $vswhere -latest -products * `
        -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 `
        -property installationPath | Select-Object -First 1)
if (-not $vsInstall) { throw "Visual Studio C++ toolset not found." }
$vsInstall = $vsInstall.Trim()

$vcvars = Join-Path $vsInstall 'VC\Auxiliary\Build\vcvars64.bat'
Assert-Path $vcvars "vcvars64.bat not found at '$vcvars'."

Write-Host "Importing MSVC environment from '$vcvars'..."
$envLines = & cmd.exe /d /s /c "`"$vcvars`" >nul 2>&1 && set"
foreach ($line in $envLines) {
    $idx = $line.IndexOf('=')
    if ($idx -gt 0) {
        Set-Item -Path ("Env:" + $line.Substring(0, $idx)) -Value $line.Substring($idx + 1)
    }
}
# vcvars points VCPKG_ROOT at the bundled vcpkg (which has no ports); restore ours.
$env:VCPKG_ROOT = $VcpkgRoot

# --- dependencies must have been installed by install-msvc.ps1 --------------
$installHint = "Run install-msvc.ps1 -BuildType $BuildType first."
Assert-Path (Join-Path $repo 'vcpkg_installed\x64-windows') `
    "The vcpkg dependencies are not installed. $installHint"
Assert-Path (Join-Path $ClunkRoot 'include\clunk') `
    "clunk is not installed at '$ClunkRoot'. $installHint"
$ffmpegMarker = if ($isDebug) {
    Join-Path $FfmpegRoot 'debug\lib'
} else {
    Join-Path $FfmpegRoot 'include\libavcodec\avcodec.h'
}
Assert-Path $ffmpegMarker "FFmpeg is not installed at '$FfmpegRoot'. $installHint"

# --- build Vangers ----------------------------------------------------------
Write-Host "`nBuilding Vangers ($BuildType)..."
cmake -S $repo -B $BuildDir -G Ninja `
    "-DCMAKE_BUILD_TYPE=$BuildType" `
    "-DCMAKE_TOOLCHAIN_FILE=$(Join-Path $VcpkgRoot 'scripts\buildsystems\vcpkg.cmake')" `
    "-DVCPKG_INSTALLED_DIR=$(Join-Path $repo 'vcpkg_installed')" `
    "-DCLUNK_ROOT=$ClunkRoot" `
    "-DFFMPEG_ROOT=$FfmpegRoot"
if ($LASTEXITCODE -ne 0) { throw "Vangers configure failed with exit code $LASTEXITCODE." }

cmake --build $BuildDir --parallel
if ($LASTEXITCODE -ne 0) { throw "Vangers build failed with exit code $LASTEXITCODE." }

Write-Host "`nBuild complete: $(Join-Path $BuildDir 'src\vangers.exe')"
