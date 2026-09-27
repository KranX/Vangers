#!/usr/bin/env bash
#
# Helpers shared by the MSYS2 build/run scripts.

# Convert a Windows path (D:\foo or D:/foo) to an MSYS2 path (/d/foo).
# Relative paths and paths that are already in MSYS2 form are returned as-is.
to_msys_path() {
	local p="$1"
	if [[ "$p" =~ ^([A-Za-z]):[\\/](.*)$ ]]; then
		local drive="${BASH_REMATCH[1],,}"
		local rest="${BASH_REMATCH[2]//\\//}"
		printf '/%s/%s' "$drive" "$rest"
	else
		printf '%s' "$p"
	fi
}
