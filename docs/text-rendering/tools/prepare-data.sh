#!/usr/bin/env bash
# Copy game assets into a NEW sandbox, without personal settings/savegames.
# Usage: bash prepare-data.sh /path/to/data /path/to/new-sandbox
set -euo pipefail
if [[ $# != 2 ]]; then
  echo 'Usage: prepare-data.sh SOURCE_DATA NEW_SANDBOX' >&2
  exit 2
fi
source_dir=$(realpath -- "$1")
target_dir=$(realpath -m -- "$2")
[[ -d "$source_dir/resource" && -d "$source_dir/iscreen" ]]
if [[ -e "$target_dir" || "$target_dir" == "$source_dir"/* ]]; then
  echo 'Target must be a new directory outside source data.' >&2
  exit 2
fi
mkdir -- "$target_dir"
shopt -s dotglob nullglob
for path in "$source_dir"/*; do
  case "${path##*/}" in
    savegame|settings.toml|options.dat|controls.dat|.baseline-isolated) continue ;;
  esac
  # Dereference asset symlinks so test writes cannot reach source data.
  cp -aL --reflink=auto -- "$path" "$target_dir/"
done
mkdir -- "$target_dir/savegame"
printf 'Isolated font-baseline data; source: %s\n' "$source_dir" > "$target_dir/.baseline-isolated"
cat > "$target_dir/settings.toml" <<'EOF'
format_version = 1
[video]
fullscreen = false
resolution = "800x600"
fps = 20
[audio]
sound_enabled = false
music_enabled = false
[input.controller]
enabled = false
[network]
player_name = "Baseline"
player_password = "BASELINE_LOCAL_ONLY"
server = "127.0.0.1"
port = 2197
EOF
