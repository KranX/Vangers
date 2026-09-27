#!/usr/bin/env bash
#
# Configure, build and test Vangers inside MSYS2. The MSYS2 environment and the
# dependencies (packages, clunk, toml11) must already be installed by
# install-msys2.sh / install-msys2.ps1.
#
# Run through build-msys2.ps1, or directly from an MSYS2 shell.
#
# Parameters are passed through the environment:
#   BUILD_TYPE   CMake build type                            (default: RelWithDebInfo)
#   BUILD_DIR    build directory, repo-relative or absolute  (default: build)
#   SKIP_TESTS   set to 1 to skip ctest                      (default: 0)

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO="$(cd "${SCRIPT_DIR}/../.." && pwd)"

BUILD_TYPE="${BUILD_TYPE:-RelWithDebInfo}"
BUILD_DIR="${BUILD_DIR:-build}"
SKIP_TESTS="${SKIP_TESTS:-0}"

MSYSTEM="${MSYSTEM:-UCRT64}"
MINGW_PREFIX="${MINGW_PREFIX:-}"

if [ -z "${MINGW_PREFIX}" ]; then
	echo "MINGW_PREFIX is not set. Run this from an MSYS2 shell or use build-msys2.ps1." >&2
	exit 1
fi

if [ ! -d "${MINGW_PREFIX}/include/clunk" ] || [ ! -d "${MINGW_PREFIX}/include/toml11" ]; then
	echo "ERROR: clunk/toml11 are not installed into ${MINGW_PREFIX}." >&2
	echo "Install the dependencies first: install-msys2.ps1 (or install-msys2.sh)." >&2
	exit 1
fi

echo "==> MSYS2 environment: ${MSYSTEM} (${MINGW_PREFIX})"
echo "==> Repository:        ${REPO}"

# --- Vangers -----------------------------------------------------------------
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
