#ifndef MCLC_SYSTEM_H_
#define MCLC_SYSTEM_H_

#include <string>

namespace MCLC {
    namespace System {
        std::string get_os();
        std::string get_macos_version();
        std::string get_arch();
    }
} //MCLC

#endif //MCLC_SYSTEM_H_