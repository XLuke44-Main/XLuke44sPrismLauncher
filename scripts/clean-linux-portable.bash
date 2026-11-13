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
DIST_DIR="${SOURCE_DIR}/dist/linux-portable"
ARCHIVE="${SOURCE_DIR}/XLuke44s-Prismlauncher-1.0.4.0-linux-portable.tar.gz"

printf 'Cleaning Linux-portable build artifacts in: %s\n' "${SOURCE_DIR}"

rm -rf -- "${BUILD_DIR}" "${DIST_DIR}"
rm -f -- "${ARCHIVE}"

rmdir -- "${SOURCE_DIR}/dist" 2>/dev/null || true

printf 'Cleanup complete.\n'
