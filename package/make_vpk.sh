#!/bin/sh
set -e
# Simple VPK packer for this project
OUT="htmlui.vpk"
TMP="./vpk_tmp"

echo "Preparing VPK in ${TMP}..."
rm -rf "${TMP}"
mkdir -p "${TMP}"

# Ensure built ELF exists
if [ -f htmlui.elf ]; then
  cp htmlui.elf "${TMP}/eboot.bin"
else
  echo "Error: htmlui.elf not found. Run 'make' first to build the ELF."
  exit 1
fi

# Copy param.sfo
mkdir -p "${TMP}/sce_sys"
if [ -f package/param.sfo ]; then
  cp package/param.sfo "${TMP}/sce_sys/param.sfo"
else
  echo "Warning: package/param.sfo not found. Please add it."
fi

# Icon handling: copy provided icon or generate a placeholder (requires ImageMagick)
mkdir -p package/icons
if [ -f package/icons/icon0.png ]; then
  cp package/icons/icon0.png "${TMP}/sce_sys/icon0.png"
else
  if command -v convert >/dev/null 2>&1; then
    echo "Generating placeholder icon (package/icons/icon0.png) using ImageMagick..."
    convert -size 960x544 xc:#222 -fill white -gravity center -pointsize 84 -annotate 0 'HTMLUI' package/icons/icon0.png
    cp package/icons/icon0.png "${TMP}/sce_sys/icon0.png"
  else
    echo "No icon found and ImageMagick not available; continuing without icon."
  fi
fi

# (Optional) Add other sce_sys files (icon1/icon2, license) if you want:
# cp package/icons/icon1.png "${TMP}/sce_sys/icon1.png" || true

# Create zip (VPK is just a zip of the package contents)
cd "${TMP}"
zip -r "../${OUT}" ./*
cd ..

echo "Created ${OUT} in repository root. Install with VitaShell."
