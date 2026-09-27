#Requires -Version 5.1
<#
.SYNOPSIS
    Install (if needed) and build Vangers with MSYS2, mirroring the
    "OSS Windows 64bit Build" GitHub Actions job.

.DESCRIPTION
    Ensures an MSYS2 installation is present (downloading and running the
    official installer when missing), then runs build-msys2.sh inside the
    selected MSYS2 environment.

.PARAMETER Msys2Root
    MSYS2 installation directory. Default: C:\msys64

.PARAMETER NoInstallMsys2
    Fail instead of installing MSYS2 when it is missing.

.PARAMETER Msys2Version
    Optional MSYS2 installer release, e.g. 20241208. Defaults to the latest.

.PARAMETER Msys2InstallerUrl
    Optional direct URL of the MSYS2 installer. Overrides -Msys2Version.

.PARAMETER Msys2Sha256
    Optional SHA256 of the installer; verified when provided.

.PARAMETER NoUpdate
    Do not update MSYS2. The required packages must already be installed,
    otherwise the script fails instead of performing a partial upgrade.

.EXAMPLE
    .\build-msys2.ps1
.EXAMPLE
    .\build-msys2.ps1 -SkipTests -NoUpdate
#>
[CmdletBinding()]
param(
    [string]$Msys2Root = 'C:\msys64',
    [switch]$NoInstallMsys2,
    [string]$Msys2Version,
    [string]$Msys2InstallerUrl,
    [string]$Msys2Sha256,
    [string]$BuildType = 'RelWithDebInfo',
    [string]$BuildDir = 'build',
    [string]$ClunkRepo = 'https://github.com/stalkerg/clunk.git',
    [string]$ClunkCommit = '3fa1e999ecc0ddb2f8eb550ca8203c7229127196',
    [string]$Toml11Version = '4.4.0',
    [switch]$SkipTests,
    [switch]$NoUpdate
)

$ErrorActionPreference = 'Stop'

function Get-Msys2InstallerUrl {
    param([string]$Version, [string]$Url)

    if ($Url) { return $Url }
    if ($Version) {
        return "https://github.com/msys2/msys2-installer/releases/download/$Version/msys2-x86_64-$Version.exe"
    }
    return 'https://github.com/msys2/msys2-installer/releases/latest/download/msys2-x86_64-latest.exe'
}

function Install-Msys2 {
    param([string]$Root, [string]$Url, [string]$Sha256)

    $cacheDir = Join-Path $env:TEMP 'msys2-installer'
    New-Item -ItemType Directory -Force -Path $cacheDir | Out-Null

    $installer = Join-Path $cacheDir ([System.IO.Path]::GetFileName($Url))
    if (-not (Test-Path $installer)) {
        Write-Host "Downloading MSYS2 installer: $Url"
        $previous = $ProgressPreference
        $ProgressPreference = 'SilentlyContinue'
        try {
            Invoke-WebRequest -Uri $Url -OutFile $installer -UseBasicParsing
        } finally {
            $ProgressPreference = $previous
        }
    }

    if ($Sha256) {
        $actual = (Get-FileHash -Algorithm SHA256 -Path $installer).Hash
        if ($actual -ne $Sha256.ToUpperInvariant()) {
            throw "MSYS2 installer checksum mismatch: expected $Sha256, got $actual."
        }
    }

    $rootForward = $Root -replace '\\', '/'
    Write-Host "Installing MSYS2 into '$Root' (this may take a few minutes)..."
    $proc = Start-Process -FilePath $installer `
        -ArgumentList @('in', '--confirm-command', '--accept-messages', '--root', $rootForward) `
        -Wait -PassThru
    if ($proc.ExitCode -ne 0) {
        throw "MSYS2 installer failed with exit code $($proc.ExitCode). Installing to '$Root' may require administrator rights."
    }
}

# --- ensure MSYS2 -----------------------------------------------------------
$bash = Join-Path $Msys2Root 'usr\bin\bash.exe'
if (-not (Test-Path $bash)) {
    if ($NoInstallMsys2) {
        throw "MSYS2 not found at '$Msys2Root'. Install it or omit -NoInstallMsys2."
    }
    $url = Get-Msys2InstallerUrl -Version $Msys2Version -Url $Msys2InstallerUrl
    Install-Msys2 -Root $Msys2Root -Url $url -Sha256 $Msys2Sha256
    if (-not (Test-Path $bash)) {
        throw "MSYS2 installation did not produce '$bash'."
    }
}

# --- update MSYS2 (own process; a core update needs a fresh shell) ----------
if (-not $NoUpdate) {
    Write-Host 'Updating MSYS2 (pacman -Syu)...'
    & $bash --login -c 'pacman --noconfirm -Syu'
    if ($LASTEXITCODE -ne 0) {
        throw "pacman -Syu failed with exit code $LASTEXITCODE."
    }
}

# --- run the build inside MSYS2 --------------------------------------------
$env:MSYSTEM = 'UCRT64'
$env:BUILD_TYPE = $BuildType
$env:BUILD_DIR = $BuildDir
$env:CLUNK_REPO = $ClunkRepo
$env:CLUNK_COMMIT = $ClunkCommit
$env:TOML11_VERSION = $Toml11Version
$env:SKIP_TESTS = if ($SkipTests) { '1' } else { '0' }
$env:UPDATE = if ($NoUpdate) { '0' } else { '1' }
$env:MSYS2_UPDATED = if ($NoUpdate) { '0' } else { '1' }

$script = (Join-Path $PSScriptRoot 'build-msys2.sh') -replace '\\', '/'
Write-Host "Running '$script' in MSYS2 ($env:MSYSTEM)..."
& $bash --login $script
if ($LASTEXITCODE -ne 0) {
    throw "build-msys2.sh failed with exit code $LASTEXITCODE."
}
