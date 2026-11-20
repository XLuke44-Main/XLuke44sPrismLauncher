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
PROJECT_VERSION="$(sed -nE 's/^project\(Launcher VERSION ([0-9]+\.[0-9]+\.[0-9]+\.[0-9]+)\).*/\1/p' "${SOURCE_DIR}/CMakeLists.txt" | head -n 1)"
if [[ -z "${PROJECT_VERSION}" ]]; then
    printf 'ERROR: could not determine the project version from CMakeLists.txt.\n' >&2
    exit 1
fi
ARCHIVE="${SOURCE_DIR}/XLuke44s-Prismlauncher-${PROJECT_VERSION}-linux-portable.tar.gz"

rm -rf "$BUILD_DIR"
rm -rf "${DIST_DIR}"
rm -f -- "${ARCHIVE}"
find "${SOURCE_DIR}" -maxdepth 1 -type f -name 'XLuke44s-Prismlauncher-*-linux-portable.tar.gz' -delete

printf 'CLEANUP COMPLETE!\n'
