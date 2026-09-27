#!/usr/bin/env bash
#
# Build Vangers and its dependencies inside MSYS2, mirroring the
# "OSS Windows 64bit Build" GitHub Actions job.
#
# This script is meant to be run inside an MSYS2 environment (UCRT64 by default)
# or from the Windows side through build-msys2.ps1, which sets MSYSTEM and
# invokes this file with `bash --login`.
#
# Parameters are passed through the environment:
#   BUILD_TYPE      CMake build type              (default: RelWithDebInfo)
#   BUILD_DIR       build directory, repo-relative or absolute (default: build)
#   CLUNK_REPO      clunk git remote              (default: stalkerg/clunk)
#   CLUNK_COMMIT    clunk commit to build         (default: CI pin)
#   TOML11_VERSION  toml11 version                (default: 4.4.0)
#   TOML11_SHA256   toml11 tarball sha256         (default: CI value)
#   SKIP_TESTS      set to 1 to skip ctest        (default: 0)
#   UPDATE          set to 0 to skip the MSYS2 update and only verify that
#                   the required packages are already installed (default: 1)

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO="$(cd "${SCRIPT_DIR}/../.." && pwd)"

# Absolute path of this script, used to re-exec after a core update.
SELF="${SCRIPT_DIR}/$(basename "${BASH_SOURCE[0]}")"

BUILD_TYPE="${BUILD_TYPE:-RelWithDebInfo}"
BUILD_DIR="${BUILD_DIR:-build}"
CLUNK_REPO="${CLUNK_REPO:-https://github.com/stalkerg/clunk.git}"
CLUNK_COMMIT="${CLUNK_COMMIT:-b52d1fda2237ef9ec81d09664ad2bb3aadd0de68}"
TOML11_VERSION="${TOML11_VERSION:-4.4.0}"
TOML11_SHA256="${TOML11_SHA256:-815bfe6792aa11a13a133b86e7f0f45edc5d71eb78f5fb6686c49c7f792b9049}"
SKIP_TESTS="${SKIP_TESTS:-0}"
UPDATE="${UPDATE:-1}"

MSYSTEM="${MSYSTEM:-UCRT64}"
MINGW_PREFIX="${MINGW_PREFIX:-}"

if [ -z "${MINGW_PREFIX}" ]; then
	echo "MINGW_PREFIX is not set. Run this from an MSYS2 shell or use build-msys2.ps1." >&2
	exit 1
fi

case "${MSYSTEM}" in
	UCRT64) MINGW_PKG_PREFIX="mingw-w64-ucrt-x86_64" ;;
	MINGW64) MINGW_PKG_PREFIX="mingw-w64-x86_64" ;;
	CLANG64) MINGW_PKG_PREFIX="mingw-w64-clang-x86_64" ;;
	*)
		echo "Unsupported MSYSTEM '${MSYSTEM}' (expected UCRT64, MINGW64 or CLANG64)." >&2
		exit 1
		;;
esac

EXTERNAL="${REPO}/external"
mkdir -p "${EXTERNAL}"

echo "==> MSYS2 environment: ${MSYSTEM} (${MINGW_PREFIX})"
echo "==> Repository:        ${REPO}"

# --- 1. packages ------------------------------------------------------------
PACKAGES=(
	git
	diffutils
	"${MINGW_PKG_PREFIX}-sdl3"
	"${MINGW_PKG_PREFIX}-sdl3-net"
	"${MINGW_PKG_PREFIX}-cmake"
	"${MINGW_PKG_PREFIX}-ffmpeg"
	"${MINGW_PKG_PREFIX}-gcc"
	"${MINGW_PKG_PREFIX}-libogg"
	"${MINGW_PKG_PREFIX}-libvorbis"
	"${MINGW_PKG_PREFIX}-ninja"
	"${MINGW_PKG_PREFIX}-pkgconf"
	"${MINGW_PKG_PREFIX}-zlib"
)

if [ "${UPDATE}" = "1" ]; then
	if [ "${MSYS2_UPDATED:-0}" != "1" ]; then
		# Update in its own process. A core runtime update requires a fresh
		# shell afterwards, so re-exec this script once with the new runtime.
		echo "==> Updating MSYS2 packages (pacman -Syu)..."
		pacman --noconfirm -Syu
		export MSYS2_UPDATED=1
		echo "==> Restarting the shell to finish the update with the new runtime..."
		exec bash --login "${SELF}"
	fi

	echo "==> Finishing the MSYS2 update (pacman -Su)..."
	pacman --noconfirm -Su

	echo "==> Installing build dependencies..."
	pacman --noconfirm --needed -S "${PACKAGES[@]}"
else
	# Never refresh the database here: `pacman -Sy` followed by `-S` would be
	# a partial upgrade, which MSYS2 does not support. Only check what exists.
	echo "==> -NoUpdate: verifying that build dependencies are already installed..."
	missing="$(pacman -T -- "${PACKAGES[@]}" 2>/dev/null || true)"
	if [ -n "${missing}" ]; then
		echo "ERROR: missing packages: ${missing}" >&2
		echo "Run this script without -NoUpdate to perform a full 'pacman -Syu'," >&2
		echo "or install the packages listed above manually." >&2
		exit 1
	fi
fi

# --- 2. clunk ---------------------------------------------------------------
CLUNK_DIR="${EXTERNAL}/clunk"
if [ ! -d "${CLUNK_DIR}/.git" ]; then
	git init "${CLUNK_DIR}"
	git -C "${CLUNK_DIR}" remote add origin "${CLUNK_REPO}"
else
	git -C "${CLUNK_DIR}" remote set-url origin "${CLUNK_REPO}"
fi

echo "==> Fetching clunk ${CLUNK_COMMIT}..."
git -C "${CLUNK_DIR}" fetch --depth 1 origin "${CLUNK_COMMIT}"
git -C "${CLUNK_DIR}" checkout --detach FETCH_HEAD

echo "==> Building clunk..."
cmake -S "${CLUNK_DIR}" -B "${CLUNK_DIR}/build" -G Ninja \
	-DCMAKE_BUILD_TYPE=Release \
	-DCMAKE_INSTALL_PREFIX="${MINGW_PREFIX}"
cmake --build "${CLUNK_DIR}/build" --parallel
cmake --install "${CLUNK_DIR}/build"

# --- 3. toml11 --------------------------------------------------------------
TOML11_DIR="${EXTERNAL}/toml11-${TOML11_VERSION}"
TOML11_TAR="${EXTERNAL}/toml11-${TOML11_VERSION}.tar.gz"

if [ ! -f "${TOML11_TAR}" ]; then
	echo "==> Downloading toml11 ${TOML11_VERSION}..."
	curl --fail --location --retry 3 \
		--output "${TOML11_TAR}" \
		"https://github.com/ToruNiina/toml11/archive/refs/tags/v${TOML11_VERSION}.tar.gz"
fi
echo "${TOML11_SHA256}  ${TOML11_TAR}" | sha256sum --check --strict

if [ ! -d "${TOML11_DIR}" ]; then
	tar --extract --gzip --file "${TOML11_TAR}" --directory "${EXTERNAL}"
fi

echo "==> Building toml11..."
cmake -Wno-deprecated \
	-S "${TOML11_DIR}" -B "${EXTERNAL}/toml11-build" -G Ninja \
	-DCMAKE_INSTALL_PREFIX="${MINGW_PREFIX}" \
	-DBUILD_TESTING=OFF \
	-DTOML11_PRECOMPILE=OFF
cmake --build "${EXTERNAL}/toml11-build" --parallel
cmake --install "${EXTERNAL}/toml11-build"

# --- 4. Vangers -------------------------------------------------------------
# Accept Windows paths too (D:\foo -> /d/foo); relative paths stay relative.
BUILD_DIR="$(cygpath --unix -- "${BUILD_DIR}")"
if [[ "${BUILD_DIR}" = /* ]]; then
	BUILD_PATH="${BUILD_DIR}"
else
	BUILD_PATH="${REPO}/${BUILD_DIR}"
fi

echo "==> Configuring Vangers (${BUILD_TYPE})..."
FRESH_FLAG=""
if [ -f "${BUILD_PATH}/CMakeCache.txt" ] \
	&& ! grep -qF "CMAKE_HOME_DIRECTORY:INTERNAL=${REPO}" "${BUILD_PATH}/CMakeCache.txt"; then
	echo "    existing CMake cache is from a different source tree; using --fresh"
	FRESH_FLAG="--fresh"
fi
cmake ${FRESH_FLAG} -S "${REPO}" -B "${BUILD_PATH}" -G Ninja \
	-DCMAKE_BUILD_TYPE="${BUILD_TYPE}"

echo "==> Building Vangers..."
cmake --build "${BUILD_PATH}" --parallel

if [ "${SKIP_TESTS}" != "1" ]; then
	echo "==> Running tests..."
	ctest --test-dir "${BUILD_PATH}" --output-on-failure
fi

echo "==> Build complete: ${BUILD_PATH}/src/vangers.exe"
