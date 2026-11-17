#!/bin/bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"

if [[ -f "${SCRIPT_DIR}/CMakeLists.txt" ]]; then
    SOURCE_DIR="${SCRIPT_DIR}"
elif [[ -f "${SCRIPT_DIR}/../CMakeLists.txt" ]]; then
    SOURCE_DIR="$(cd -- "${SCRIPT_DIR}/.." && pwd)"
else
    printf 'ERROR: could not locate the project root (CMakeLists.txt).\n' >&2
    exit 1
fi

BUILD_DIR="${SOURCE_DIR}/build-linux-portable"
DIST_DIR="${SOURCE_DIR}/dist"
ARCHIVE="${SOURCE_DIR}/XLuke44s-Prismlauncher-1.0.4.1-linux-portable.tar.gz"

rm -rf "$BUILD_DIR"
rm -rf "${DIST_DIR}"
rm -f -- "${ARCHIVE}"

printf 'CLEANUP COMPLETE!\n'
