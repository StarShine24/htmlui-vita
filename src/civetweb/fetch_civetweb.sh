#!/bin/sh
# Fetch civetweb single-file source and header into src/civetweb/
set -e
mkdir -p src/civetweb
cd src/civetweb
if [ -f civetweb.c ] && [ -f civetweb.h ]; then
  echo "civetweb already present"
  exit 0
fi
curl -L https://raw.githubusercontent.com/civetweb/civetweb/master/civetweb.c -o civetweb.c
curl -L https://raw.githubusercontent.com/civetweb/civetweb/master/include/civetweb.h -o civetweb.h
echo "Downloaded civetweb.c and civetweb.h into src/civetweb/"
