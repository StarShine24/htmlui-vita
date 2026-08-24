# htmlui-vita

Standalone PS Vita homebrew that runs an embedded HTTP + WebSocket server (civetweb) and serves a local HTML/CSS/JS UI you open in the Vita system browser.

This repository contains:

- src/main.cpp — minimal Vita app that starts civetweb and registers an HTTP POST endpoint (/api/native) and a WebSocket endpoint (/ws).
- src/civetweb/fetch_civetweb.sh — helper script to download civetweb.c and civetweb.h into the tree.
- data/index.html, data/app.js, data/style.css — example HTML/CSS/JS UI that talks to native code.
- Makefile — builds an ELF using VitaSDK toolchain (see README). Packaging into a VPK is documented below.
- README.md — setup and usage instructions.

License: MIT
