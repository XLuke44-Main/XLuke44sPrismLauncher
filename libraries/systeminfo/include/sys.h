// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include <cstdint>

namespace Sys {
constexpr uint64_t mebibyte = 1024ull * 1024ull;
uint64_t getSystemRam();
}
