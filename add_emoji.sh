#!/usr/bin/env bash
set -euo pipefail

if [ "$#" -lt 2 ]; then
    echo "Usage: $0 <emoji> <name> (e.g., $0 ☀️ sun)"
    exit 1
fi

EMOJI="$1"
NAME="${2%.svg}"
OUT_DIR="assets/emojis"
mkdir -p "$OUT_DIR"

CODEPOINT=$(python3 -c "import sys; print('_'.join(f'{ord(c):x}' for c in sys.argv[1] if ord(c) != 0xfe0f))" "$EMOJI")

URL="https://raw.githubusercontent.com/googlefonts/noto-emoji/main/2D/svg/emoji_u${CODEPOINT}.svg"
DEST="${OUT_DIR}/${NAME}.svg"

echo "Downloading ${EMOJI}."
if curl -fsSL "$URL" -o "$DEST"; then
    echo "Saved ${DEST}."
else
    echo "Error: Failed to download ${URL}."
    exit 1
fi