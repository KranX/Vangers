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
#   BUILD_DIR       build directory, repo-relative(default: build)
#   CLUNK_REPO      clunk git remote              (default: stalkerg/clunk)
#   CLUNK_COMMIT    clunk commit to build         (default: CI pin)
#   TOML11_VERSION  toml11 version                (default: 4.4.0)
#   TOML11_SHA256   toml11 tarball sha256         (default: CI value)
#   SKIP_TESTS      set to 1 to skip ctest        (default: 0)
#   UPDATE          set to 0 to skip `pacman -Syu` (default: 1)

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO="$(cd "${SCRIPT_DIR}/../.." && pwd)"

BUILD_TYPE="${BUILD_TYPE:-RelWithDebInfo}"
BUILD_DIR="${BUILD_DIR:-build}"
CLUNK_REPO="${CLUNK_REPO:-https://github.com/stalkerg/clunk.git}"
CLUNK_COMMIT="${CLUNK_COMMIT:-3fa1e999ecc0ddb2f8eb550ca8203c7229127196}"
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
if [ "${UPDATE}" = "1" ]; then
	echo "==> Updating MSYS2 packages (pacman -Syu)..."
	pacman --noconfirm -Syu
else
	echo "==> Refreshing the MSYS2 package database (pacman -Sy)..."
	pacman --noconfirm -Sy
fi

echo "==> Installing build dependencies..."
pacman --noconfirm --needed -S \
	git diffutils \
	"${MINGW_PKG_PREFIX}-sdl3" \
	"${MINGW_PKG_PREFIX}-sdl3-net" \
	"${MINGW_PKG_PREFIX}-cmake" \
	"${MINGW_PKG_PREFIX}-ffmpeg" \
	"${MINGW_PKG_PREFIX}-gcc" \
	"${MINGW_PKG_PREFIX}-libogg" \
	"${MINGW_PKG_PREFIX}-libvorbis" \
	"${MINGW_PKG_PREFIX}-ninja" \
	"${MINGW_PKG_PREFIX}-pkgconf" \
	"${MINGW_PKG_PREFIX}-zlib"

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
if [[ "${BUILD_DIR}" = /* || "${BUILD_DIR}" =~ ^[A-Za-z]:/ ]]; then
	BUILD_PATH="${BUILD_DIR}"
else
	BUILD_PATH="${REPO}/${BUILD_DIR}"
fi

echo "==> Configuring Vangers (${BUILD_TYPE})..."
cmake -S "${REPO}" -B "${BUILD_PATH}" -G Ninja \
	-DCMAKE_BUILD_TYPE="${BUILD_TYPE}"

echo "==> Building Vangers..."
cmake --build "${BUILD_PATH}" --parallel

if [ "${SKIP_TESTS}" != "1" ]; then
	echo "==> Running tests..."
	ctest --test-dir "${BUILD_PATH}" --output-on-failure
fi

echo "==> Build complete: ${BUILD_PATH}/src/vangers.exe"
