#!/usr/bin/env bash

set -euo pipefail

if [[ $# -lt 2 || $# -gt 3 ]]; then
    echo "Usage: $0 \"Title\" EPISODE [SUBTITLE]"
    exit 1
fi

title="$1"
episode="$2"
subtitle="${3:-}"
out="thumb_ep_${episode}.png"
#font="$HOME/Library/Fonts/BerkeleyMono-Regular.ttf"
font="/usr/local/share/fonts/b/BerkeleyMono_Regular.ttf"

char_count=${#title}
if   [[ $char_count -le 15 ]]; then pointsize=96
elif [[ $char_count -le 22 ]]; then pointsize=80
elif [[ $char_count -le 30 ]]; then pointsize=64
else                                pointsize=52
fi

title_offset=$(( pointsize / 2 + 28 ))

cmd=(
    magick
    -size 1280x720 xc:"#111111"
    \( +clone -seed 1 -attenuate 0.4 +noise Gaussian -colorspace gray -evaluate multiply 0.25 \)
    -compose screen -composite
    -font "$font"
    -fill "#f0f0ee"
    -pointsize "$pointsize"
    -gravity center
    -annotate "+0-$title_offset" "$title"
    -fill "#4a9e6b"
    -draw "rectangle 160,368 1120,371"
)

if [[ -n "$subtitle" ]]; then
    cmd+=(
        -fill "#cfd6cf"
        -pointsize 30
        -gravity center
        -annotate "+0+62" "$subtitle"
    )
fi

cmd+=(
    -fill "#4a9e6b"
    -pointsize 42
    -gravity southeast
    -annotate "+50+50" "Terrarium Ep $episode"
    "$out"
)

"${cmd[@]}"

echo "Wrote $out"
