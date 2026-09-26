#Requires -Version 5.1
<#
.SYNOPSIS
    Build Vangers with MSVC and all its dependencies.

.DESCRIPTION
    1. installs the vcpkg manifest dependencies into <repo>\vcpkg_installed
    2. builds and installs clunk (from -ClunkSrc) into -ClunkRoot
    3. configures and builds Vangers into -BuildDir

    All paths can be overridden; the defaults match the documented MSVC setup.
#>
[CmdletBinding()]
param(
    [string]$VcpkgRoot = $env:VCPKG_ROOT,
    [string]$ClunkSrc,
    [string]$ClunkRoot,
    [string]$FfmpegRoot,
    [string]$BuildDir,
    [string]$BuildType = 'RelWithDebInfo'
)

$ErrorActionPreference = 'Stop'

$repo = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path

if (-not $VcpkgRoot)  { $VcpkgRoot  = Join-Path $env:USERPROFILE 'vcpkg' }
if (-not $ClunkSrc)   { $ClunkSrc   = Join-Path $repo 'clunk' }
if (-not $ClunkRoot)  { $ClunkRoot  = Join-Path $env:USERPROFILE 'clunk-install' }
if (-not $FfmpegRoot) { $FfmpegRoot = Join-Path $repo 'ffmpeg' }
if (-not $BuildDir)   { $BuildDir   = Join-Path $repo 'build-msvc' }

function Assert-Path([string]$Path, [string]$Message) {
    if (-not (Test-Path $Path)) { throw $Message }
}

Assert-Path (Join-Path $VcpkgRoot 'vcpkg.exe') `
    "vcpkg not found at '$VcpkgRoot'. Install it from https://learn.microsoft.com/vcpkg/get_started/get-started or set -VcpkgRoot."
Assert-Path (Join-Path $FfmpegRoot 'include\libavcodec\avcodec.h') `
    "Prebuilt MSVC FFmpeg not found at '$FfmpegRoot'. Download a shared x64 build from https://github.com/System233/ffmpeg-msvc-prebuilt and set -FfmpegRoot."
Assert-Path (Join-Path $ClunkSrc 'CMakeLists.txt') `
    "clunk sources not found at '$ClunkSrc'. Clone the msvc branch of https://github.com/DileSoft/clunk or set -ClunkSrc."

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
$env:VCPKG_BUILD_TYPE = 'release'
$vcpkgPrefix = Join-Path $repo 'vcpkg_installed\x64-windows'

# --- 1. vcpkg dependencies --------------------------------------------------
Write-Host "`n[1/3] Installing vcpkg dependencies (triplet x64-windows, release)..."
& (Join-Path $VcpkgRoot 'vcpkg.exe') install --triplet x64-windows `
    "--x-manifest-root=$repo" "--x-install-root=$(Join-Path $repo 'vcpkg_installed')"
if ($LASTEXITCODE -ne 0) { throw "vcpkg install failed with exit code $LASTEXITCODE." }

# --- 2. clunk ---------------------------------------------------------------
Write-Host "`n[2/3] Building clunk from '$ClunkSrc'..."
$clunkBuild = Join-Path $ClunkSrc 'build'
cmake -S $ClunkSrc -B $clunkBuild -G Ninja `
    -DCMAKE_BUILD_TYPE=Release `
    "-DCMAKE_INSTALL_PREFIX=$ClunkRoot" `
    "-DCMAKE_PREFIX_PATH=$vcpkgPrefix"
if ($LASTEXITCODE -ne 0) { throw "clunk configure failed with exit code $LASTEXITCODE." }

cmake --build $clunkBuild --parallel
if ($LASTEXITCODE -ne 0) { throw "clunk build failed with exit code $LASTEXITCODE." }

cmake --install $clunkBuild
if ($LASTEXITCODE -ne 0) { throw "clunk install failed with exit code $LASTEXITCODE." }

# --- 3. Vangers -------------------------------------------------------------
Write-Host "`n[3/3] Building Vangers ($BuildType)..."
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
