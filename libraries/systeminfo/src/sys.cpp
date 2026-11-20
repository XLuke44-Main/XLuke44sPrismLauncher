// SPDX-License-Identifier: GPL-3.0-only
#include "sys.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <fstream>
#include <limits>
#include <string>
#endif

namespace Sys {
uint64_t getSystemRam()
{
#ifdef _WIN32
    MEMORYSTATUSEX status{};
    status.dwLength = sizeof(status);
    return GlobalMemoryStatusEx(&status) ? status.ullTotalPhys : 0;
#else
    std::ifstream file("/proc/meminfo");
    std::string key;
    while (file >> key) {
        if (key == "MemTotal:") {
            uint64_t kibibytes = 0;
            return file >> kibibytes ? kibibytes * 1024ull : 0;
        }
        file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    return 0;
#endif
}
}
