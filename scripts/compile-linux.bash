#!/bin/bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
"${SCRIPT_DIR}/clean-linux-portable.bash"
"${SCRIPT_DIR}/build-linux-portable.bash"
