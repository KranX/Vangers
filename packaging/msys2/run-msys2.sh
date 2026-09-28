#!/usr/bin/env bash
#
# Copy the built Vangers binaries and their MSYS2 runtime DLLs into a game
# directory. Run from MSYS2 or through run-msys2.ps1.
#
# Environment:
#   GAME_DIR     target game directory (required)
#   BUILD_DIR    build directory, repo-relative or absolute (default: build)

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO="$(cd "${SCRIPT_DIR}/../.." && pwd)"

GAME_DIR="${GAME_DIR:-}"
BUILD_DIR="${BUILD_DIR:-build}"
MINGW_PREFIX="${MINGW_PREFIX:-/ucrt64}"

if [ -z "${GAME_DIR}" ]; then
	echo "GAME_DIR is not set. Specify a game directory." >&2
	exit 1
fi

# Accept Windows paths too (D:\foo -> /d/foo); relative paths stay relative.
GAME_DIR="$(cygpath --unix -- "${GAME_DIR}")"
BUILD_DIR="$(cygpath --unix -- "${BUILD_DIR}")"

if [[ "${BUILD_DIR}" = /* ]]; then
	BUILD_PATH="${BUILD_DIR}"
else
	BUILD_PATH="${REPO}/${BUILD_DIR}"
fi

if [ ! -f "${BUILD_PATH}/src/vangers.exe" ]; then
	echo "vangers.exe not found in '${BUILD_PATH}/src'. Build the project first." >&2
	exit 1
fi

mkdir -p "${GAME_DIR}"

echo "==> Copying game to ${GAME_DIR}"
cp -f "${BUILD_PATH}/src/vangers.exe" "${GAME_DIR}/"
if [ -f "${BUILD_PATH}/surmap/surmap.exe" ]; then
	cp -f "${BUILD_PATH}/surmap/surmap.exe" "${GAME_DIR}/"
fi

echo "==> Copying MSYS2 runtime DLLs"
# Resolve dependencies from the build tree (not the destination), otherwise
# ldd would pick up stale DLLs already present in the game directory.
collect_dlls() {
	local exe="$1"
	[ -f "${exe}" ] || return 0
	ldd "${exe}" | awk '$3 ~ /\/(ucrt64|mingw64|clang64)\// { print $3 }'
}

for exe in "${BUILD_PATH}/src/vangers.exe" "${BUILD_PATH}/surmap/surmap.exe"; do
	collect_dlls "${exe}" | sort -u | while read -r dll; do
		if [ -f "${dll}" ]; then
			cp -f "${dll}" "${GAME_DIR}/"
		fi
	done
done

# Always ship a consistent C++ runtime, even if the destination had stale ones.
for dll in libwinpthread-1.dll libstdc++-6.dll libgcc_s_seh-1.dll libgcc_s_dw2-1.dll; do
	if [ -f "${MINGW_PREFIX}/bin/${dll}" ]; then
		cp -f "${MINGW_PREFIX}/bin/${dll}" "${GAME_DIR}/"
	fi
done

echo "==> Done: ${GAME_DIR}"
