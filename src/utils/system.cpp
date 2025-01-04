#include "system.h"

#if defined(__APPLE__) || defined(__MACH__)
#include <sys/sysctl.h>
#endif

namespace MCLC {
    std::string System::get_os() {
    #if defined(_WIN32) || defined(_WIN64)
        return "windows";
    #elif defined(__APPLE__) || defined(__MACH__)
        return "osx";
    #elif defined(__linux__)
        return "linux";
    #else
        return "unknown";
    #endif
    }

    std::string System::get_arch() {
    #if defined(_WIN64) || defined(__x86_64__) || defined(__ppc64__) || defined(_M_X64)
        return ""; //Don't need to mark it explicitly
    #elif defined(_WIN32) || defined(__i386__) || defined(__ppc__) || defined(_M_IX86)
        return "x86";
    #elif defined(_M_ARM64) || defined(__aarch64__)
        return "arm64";
    #else
        return "unknown";
    #endif
    }

    std::string System::get_macos_version() {
    #if defined(__APPLE__) || defined(__MACH__)
        int mib[2];
        size_t len;
        char version[256];

        mib[0] = CTL_KERN;
        mib[1] = KERN_OSVERSION;
        len = sizeof(version);

        if (sysctl(mib, 2, version, &len, NULL, 0) == 0) {
            return std::string(version);
        }
    #endif
        return "0.0";
    }
};