#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
if [[ -f "${SCRIPT_DIR}/CMakeLists.txt" ]]; then
    SOURCE_DIR="${SCRIPT_DIR}"
elif [[ -f "${SCRIPT_DIR}/../CMakeLists.txt" ]]; then
    SOURCE_DIR="$(cd -- "${SCRIPT_DIR}/.." && pwd)"
else
    printf 'ERROR: could not locate the XLuke44 Prismlauncher source root (CMakeLists.txt).\n' >&2
    exit 1
fi

JAVA_HOME="${JAVA_HOME:-/home/xluke44/BIN/JAVA/jdk1.8.0_51}"
BUILD_DIR="${SOURCE_DIR}/build-linux-portable"
DIST_DIR="${SOURCE_DIR}/dist/linux-portable"
ARCHIVE="${SOURCE_DIR}/XLuke44s-Prismlauncher-1.0.3.1-linux-portable.tar.gz"

for tool in java javac jar; do
    if [[ ! -x "${JAVA_HOME}/bin/${tool}" ]]; then
        printf 'ERROR: required build-time Java tool is missing or not executable: %s\n' "${JAVA_HOME}/bin/${tool}" >&2
        exit 1
    fi
done

for tool in cmake ninja tar nproc; do
    if ! command -v "${tool}" >/dev/null 2>&1; then
        printf 'ERROR: required build tool is missing: %s\n' "${tool}" >&2
        exit 1
    fi
done

export JAVA_HOME
export PATH="${JAVA_HOME}/bin:${PATH}"

# Archive timestamps may be ahead of the local clock. Ninja would then keep
# regenerating build.ninja because its CMake inputs remain newer than it.
CURRENT_EPOCH="$(date +%s)"
while IFS= read -r -d '' cmake_input; do
    touch -- "${cmake_input}"
done < <(find "${SOURCE_DIR}" -type f \
    \( -name 'CMakeLists.txt' -o -name '*.cmake' -o -name '*.cmake.in' -o -name '*.cmakein' \) \
    -newermt "@${CURRENT_EPOCH}" -print0)

# Use absolute paths so invoking this script from scripts/ (or elsewhere) is safe.
# Clean the old build and package to avoid accidentally shipping stale binaries.
rm -rf "${BUILD_DIR}" "${DIST_DIR}"
mkdir -p "${DIST_DIR}"

cmake -S "${SOURCE_DIR}" -B "${BUILD_DIR}" -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="${DIST_DIR}" \
    -DJava_JAVA_EXECUTABLE="${JAVA_HOME}/bin/java" \
    -DJava_JAVAC_EXECUTABLE="${JAVA_HOME}/bin/javac" \
    -DJava_JAR_EXECUTABLE="${JAVA_HOME}/bin/jar" \
    -DLauncher_QT_VERSION_MAJOR=5 \
    -DLauncher_BUILD_PLATFORM=linux-portable \
    -DLauncher_APP_BINARY_NAME=xluke44s-prismlauncher \
    -DLauncher_FORCE_BUNDLED_LIBS=ON \
    -DINSTALL_BUNDLE=full \

# With errexit enabled, an unsuccessful compile stops here and neither install
# nor packaging runs against an incomplete build.
cmake --build "${BUILD_DIR}" --parallel "$(nproc)"

# Runtime files and the portable runner/portable.txt are separate components.
cmake --install "${BUILD_DIR}"
cmake --install "${BUILD_DIR}" --component portable

# Include the runtime library used by the project's portable Linux package.
BZIP2_LIBRARY=""
if command -v ldconfig >/dev/null 2>&1; then
    BZIP2_LIBRARY="$(ldconfig -p 2>/dev/null | awk '$1 == "libbz2.so.1.0" { print $NF; exit }')"
fi
for candidate in /lib/x86_64-linux-gnu/libbz2.so.1.0 /usr/lib/x86_64-linux-gnu/libbz2.so.1.0 /lib64/libbz2.so.1.0 /usr/lib64/libbz2.so.1.0; do
    if [[ -z "${BZIP2_LIBRARY}" && -r "${candidate}" ]]; then
        BZIP2_LIBRARY="${candidate}"
    fi
done
if [[ -n "${BZIP2_LIBRARY}" && -r "${BZIP2_LIBRARY}" ]]; then
    mkdir -p "${DIST_DIR}/lib"
    cp -L "${BZIP2_LIBRARY}" "${DIST_DIR}/lib/libbz2.so.1.0"
else
    printf 'WARNING: libbz2.so.1.0 was not found; the portable runner may report it as a missing host dependency.\n' >&2
fi

find "${DIST_DIR}" -type f ! -name manifest.txt -printf '%P\n' | sort > "${DIST_DIR}/manifest.txt"

tar --sort=name --owner=0 --group=0 --numeric-owner -czf "${ARCHIVE}" \
    -C "${SOURCE_DIR}/dist" linux-portable

printf '\nLinux Portable package created: %s\n' "${ARCHIVE}"
