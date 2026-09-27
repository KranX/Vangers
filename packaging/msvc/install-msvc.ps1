#Requires -Version 5.1
<#
.SYNOPSIS
    Install the MSVC build dependencies for Vangers.

.DESCRIPTION
    1. installs the vcpkg manifest dependencies into <repo>\vcpkg_installed
    2. downloads a prebuilt MSVC FFmpeg into <repo>\external\ffmpeg when missing
       (the "-develop" archive for Debug, which also ships debug libraries)
    3. clones clunk into <repo>\external\clunk when missing
    4. builds and installs clunk into <repo>\external\clunk-install[-debug]

    Run this once, or after changing a dependency revision or configuration;
    build-msvc.ps1 only builds Vangers.

    Visual Studio and vcpkg must already be installed (see INSTALL): this script
    only fetches FFmpeg and clunk on its own.

    Debug is installed end to end: the vcpkg dependencies and clunk are built in
    Debug too, so the C++/STL ABI matches across the clunk DLL.
#>
[CmdletBinding()]
param(
    [string]$VcpkgRoot = $env:VCPKG_ROOT,
    [string]$ClunkSrc,
    [string]$ClunkRoot,
    [string]$FfmpegRoot,

    [ValidateSet('Release', 'RelWithDebInfo', 'MinSizeRel', 'Debug')]
    [string]$BuildType = 'RelWithDebInfo',

    [string]$ClunkRepo = 'https://github.com/stalkerg/clunk.git',
    [string]$ClunkCommit = 'b52d1fda2237ef9ec81d09664ad2bb3aadd0de68',
    [switch]$NoInstallClunk,

    [string]$FfmpegVersion = '9.0.2',
    [string]$FfmpegUrl,
    [string]$FfmpegSha256,
    [switch]$NoInstallFfmpeg
)

$ErrorActionPreference = 'Stop'

$repo = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
$external = Join-Path $repo 'external'

$isDebug = ($BuildType -eq 'Debug')
# Debug keeps its own install tree so it never mixes with the release DLLs.
$configSuffix = if ($isDebug) { '-debug' } else { '' }

if (-not $VcpkgRoot)  { $VcpkgRoot  = Join-Path $env:USERPROFILE 'vcpkg' }
if (-not $ClunkSrc)   { $ClunkSrc   = Join-Path $external 'clunk' }
if (-not $ClunkRoot)  { $ClunkRoot  = Join-Path $external "clunk-install$configSuffix" }
if (-not $FfmpegRoot) { $FfmpegRoot = Join-Path $external 'ffmpeg' }

function Assert-Path([string]$Path, [string]$Message) {
    if (-not (Test-Path $Path)) { throw $Message }
}

function Get-FfmpegUrl {
    param([string]$Version, [string]$Url, [switch]$Develop)

    if ($Url) { return $Url }
    $base = "https://github.com/System233/ffmpeg-msvc-prebuilt/releases/download/ffmpeg-$Version"
    $suffix = if ($Develop) { '-develop' } else { '' }
    return "$base/ffmpeg-${Version}_x64-windows-shared-lgpl$suffix.zip"
}

function Install-Ffmpeg {
    param([string]$Root, [string]$Url, [string]$Sha256)

    $cacheDir = Join-Path $env:TEMP 'ffmpeg-msvc-prebuilt'
    New-Item -ItemType Directory -Force -Path $cacheDir | Out-Null

    $zip = Join-Path $cacheDir ([System.IO.Path]::GetFileName($Url))
    if (-not (Test-Path $zip)) {
        Write-Host "Downloading FFmpeg (MSVC prebuilt): $Url"
        $previous = $ProgressPreference
        $ProgressPreference = 'SilentlyContinue'
        try {
            Invoke-WebRequest -Uri $Url -OutFile $zip -UseBasicParsing
        } finally {
            $ProgressPreference = $previous
        }
    }

    if ($Sha256) {
        $actual = (Get-FileHash -Algorithm SHA256 -Path $zip).Hash
        if ($actual -ne $Sha256.ToUpperInvariant()) {
            throw "FFmpeg archive checksum mismatch: expected $Sha256, got $actual."
        }
    }

    $parent = Split-Path -Parent $Root
    if ($parent) { New-Item -ItemType Directory -Force -Path $parent | Out-Null }
    Write-Host "Extracting FFmpeg into '$Root'..."
    Expand-Archive -Path $zip -DestinationPath $Root -Force
}

function Install-Clunk {
    param([string]$Src, [string]$Repo, [string]$Commit)

    if (-not (Get-Command git -ErrorAction SilentlyContinue)) {
        throw "git not found in PATH. Install Git or pass -ClunkSrc with existing clunk sources."
    }

    $parent = Split-Path -Parent $Src
    if ($parent -and -not (Test-Path $parent)) {
        New-Item -ItemType Directory -Force -Path $parent | Out-Null
    }

    if (-not (Test-Path (Join-Path $Src '.git'))) {
        New-Item -ItemType Directory -Force -Path $Src | Out-Null
        git -C $Src init --quiet
        git -C $Src remote add origin $Repo
    } else {
        git -C $Src remote set-url origin $Repo
    }

    # --verify --quiet keeps an unborn HEAD (fresh git init) from writing a
    # "fatal:" line to stderr, which would terminate the script when
    # $ErrorActionPreference is Stop.
    $current = git -C $Src rev-parse --verify --quiet HEAD 2>$null
    if ($current -ne $Commit) {
        Write-Host "Fetching clunk $Commit from $Repo..."
        git -C $Src fetch --depth 1 origin $Commit
        if ($LASTEXITCODE -ne 0) { throw "git fetch failed with exit code $LASTEXITCODE." }
        git -C $Src checkout --detach FETCH_HEAD --quiet
        if ($LASTEXITCODE -ne 0) { throw "git checkout failed with exit code $LASTEXITCODE." }
    } else {
        Write-Host "clunk is already at $Commit."
    }
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
# Only build the configuration we are going to use.
$env:VCPKG_BUILD_TYPE = if ($isDebug) { 'debug' } else { 'release' }
$vcpkgPrefix = Join-Path $repo 'vcpkg_installed\x64-windows'

# --- 1. vcpkg dependencies --------------------------------------------------
Write-Host "`n[1/4] Installing vcpkg dependencies (triplet x64-windows, $($env:VCPKG_BUILD_TYPE))..."
& (Join-Path $VcpkgRoot 'vcpkg.exe') install --triplet x64-windows `
    "--x-manifest-root=$repo" "--x-install-root=$(Join-Path $repo 'vcpkg_installed')"
if ($LASTEXITCODE -ne 0) { throw "vcpkg install failed with exit code $LASTEXITCODE." }

# --- 2. FFmpeg --------------------------------------------------------------
Write-Host "`n[2/4] Ensuring prebuilt MSVC FFmpeg at '$FfmpegRoot'..."
$ffmpegMarker = if ($isDebug) {
    Join-Path $FfmpegRoot 'debug\lib'
} else {
    Join-Path $FfmpegRoot 'include\libavcodec\avcodec.h'
}
if (-not (Test-Path $ffmpegMarker)) {
    if ($NoInstallFfmpeg) {
        throw "Prebuilt MSVC FFmpeg not found at '$FfmpegRoot'. Download a shared x64 build from https://github.com/System233/ffmpeg-msvc-prebuilt and set -FfmpegRoot."
    }
    $ffmpegUrl = Get-FfmpegUrl -Version $FfmpegVersion -Url $FfmpegUrl -Develop:$isDebug
    Install-Ffmpeg -Root $FfmpegRoot -Url $ffmpegUrl -Sha256 $FfmpegSha256
    Assert-Path $ffmpegMarker "FFmpeg extraction did not produce '$ffmpegMarker'."
}

# --- 3. clunk sources -------------------------------------------------------
$manageClunk = -not $PSBoundParameters.ContainsKey('ClunkSrc')
Write-Host "`n[3/4] Ensuring clunk sources at '$ClunkSrc'..."
if (-not (Test-Path (Join-Path $ClunkSrc 'CMakeLists.txt'))) {
    if ($NoInstallClunk) {
        throw "clunk sources not found at '$ClunkSrc'. Clone https://github.com/stalkerg/clunk or set -ClunkSrc."
    }
    Install-Clunk -Src $ClunkSrc -Repo $ClunkRepo -Commit $ClunkCommit
    Assert-Path (Join-Path $ClunkSrc 'CMakeLists.txt') `
        "clunk checkout did not produce '$ClunkSrc\CMakeLists.txt'."
} elseif ($manageClunk -and (Test-Path (Join-Path $ClunkSrc '.git'))) {
    # The default location is auto-managed, so keep it on the pinned commit.
    Install-Clunk -Src $ClunkSrc -Repo $ClunkRepo -Commit $ClunkCommit
}

# --- 4. clunk ---------------------------------------------------------------
$clunkBuildType = if ($isDebug) { 'Debug' } else { 'Release' }
Write-Host "`n[4/4] Building clunk ($clunkBuildType) from '$ClunkSrc'..."
$clunkBuild = Join-Path $ClunkSrc "build-msvc$configSuffix"
cmake -S $ClunkSrc -B $clunkBuild -G Ninja `
    "-DCMAKE_BUILD_TYPE=$clunkBuildType" `
    "-DCMAKE_INSTALL_PREFIX=$ClunkRoot" `
    "-DCMAKE_PREFIX_PATH=$vcpkgPrefix"
if ($LASTEXITCODE -ne 0) { throw "clunk configure failed with exit code $LASTEXITCODE." }

cmake --build $clunkBuild --parallel
if ($LASTEXITCODE -ne 0) { throw "clunk build failed with exit code $LASTEXITCODE." }

cmake --install $clunkBuild
if ($LASTEXITCODE -ne 0) { throw "clunk install failed with exit code $LASTEXITCODE." }

Write-Host "`nInstalled dependencies ($BuildType): clunk at $ClunkRoot, FFmpeg at $FfmpegRoot."
