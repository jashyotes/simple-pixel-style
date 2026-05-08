#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
OUT="$ROOT/design-mockups"
VENDOR="$OUT/vendor/w800"

HOUR_FONT="$VENDOR/fonts/FONT_HOUR_BIG_42.TTF"
INFO_BIG="$VENDOR/fonts/FONT_INFO_BIG_16.TTF"
INFO_SMALL="$VENDOR/fonts/FONT_INFO_SMALL_16.TTF"
FOUR_FONT="$VENDOR/fonts/FONT_FOURCHAR_16.TTF"
LOGO="$VENDOR/images/pebble-logo-white.png"
TOGGLES="$VENDOR/images/IMAGE_SHEET_TOGGLES.PNG"
BRANDING="$VENDOR/images/IMAGE_SHEET_BRANDING.PNG"

WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT

make_label() {
  local font="$1"
  local point="$2"
  local fill="$3"
  local text="$4"
  local output="$5"
  magick -background none -fill "$fill" +antialias -font "$font" -pointsize "$point" label:"$text" "$output"
}

make_label_scaled() {
  local font="$1"
  local point="$2"
  local fill="$3"
  local text="$4"
  local scale="$5"
  local output="$6"
  magick -background none -fill "$fill" +antialias -font "$font" -pointsize "$point" label:"$text" \
    -filter point -resize "$scale" "$output"
}

magick "$LOGO" -filter point -resize 135% "$WORK/pebble-logo.png"
magick "$TOGGLES" -crop 6x7+46+12 +repage -filter point -resize 135% "$WORK/pm.png"
magick "$BRANDING" -crop 76x9+52+0 +repage -filter point -resize 135% \
  -alpha set -transparent white -fill white -opaque black "$WORK/top-brand.png"
magick "$BRANDING" -crop 128x12+0+60 +repage -filter point -resize 135% \
  -alpha set -transparent white -fill white -opaque black "$WORK/bottom-brand.png"

make_label_scaled "$HOUR_FONT" 42 black "01:35" 115% "$WORK/time.png"
make_label_scaled "$FOUR_FONT" 16 black "THU" 125% "$WORK/day.png"
make_label_scaled "$INFO_BIG" 16 black "100" 125% "$WORK/watch-battery.png"
make_label_scaled "$INFO_BIG" 16 black "77F" 125% "$WORK/temp.png"
make_label "$INFO_SMALL" 16 black "STEPS" "$WORK/steps-label.png"
make_label_scaled "$INFO_BIG" 16 black "11105" 125% "$WORK/steps.png"
make_label "$INFO_SMALL" 16 black "BPM" "$WORK/bpm-label.png"
make_label_scaled "$INFO_BIG" 16 black "72" 125% "$WORK/bpm.png"
make_label "$INFO_BIG" 16 black "MAY 7" "$WORK/date.png"
make_label "$INFO_SMALL" 16 black "3PM KOALA" "$WORK/event.png"
make_label "$INFO_SMALL" 16 white "MODE" "$WORK/mode.png"
make_label "$INFO_SMALL" 16 white "ADJUST" "$WORK/adjust.png"
make_label "$INFO_SMALL" 16 white "LIGHT" "$WORK/light.png"

magick -size 200x228 xc:black \
  "$WORK/pebble-logo.png" -geometry +14+20 -composite \
  "$WORK/top-brand.png" -geometry +93+22 -composite \
  -fill "#f2f2f2" -stroke none -draw "roundrectangle 8,45 191,194 10,10" \
  -fill black -stroke none -draw "roundrectangle 11,48 188,191 7,7" \
  -fill white -stroke none -draw "roundrectangle 14,51 185,188 5,5" \
  -fill black -stroke none \
    -draw "rectangle 18,83 181,85" \
    -draw "rectangle 18,164 181,166" \
    -draw "rectangle 96,53 98,83" \
    -draw "rectangle 128,53 130,83" \
  "$WORK/day.png" -geometry +29+59 -composite \
  "$WORK/watch-battery.png" -geometry +104+60 -composite \
  "$WORK/temp.png" -geometry +139+60 -composite \
  "$WORK/steps.png" -geometry +23+90 -composite \
  "$WORK/bpm.png" -geometry +139+90 -composite \
  "$WORK/pm.png" -geometry +24+126 -composite \
  "$WORK/time.png" -geometry +43+111 -composite \
  "$WORK/date.png" -geometry +22+171 -composite \
  "$WORK/event.png" -geometry +82+172 -composite \
  "$WORK/bottom-brand.png" -geometry +14+205 -composite \
  "$OUT/w800h-emery-clone.png"

magick "$OUT/w800h-emery-clone.png" -filter point -resize 300% "$OUT/w800h-emery-clone-3x.png"
