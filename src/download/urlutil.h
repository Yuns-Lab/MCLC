#ifndef MCLC_URLPROCESS_H
#define MCLC_URLPROCESS_H

#include <string>

namespace MCLC {
    std::string get_filename(const std::string&);
    std::pair<std::string, std::string> get_host_and_path(const std::string&);
}

#endif //MCLC_URLPROCESS_H
