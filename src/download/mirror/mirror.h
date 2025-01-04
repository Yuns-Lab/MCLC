#ifndef MCLC_MIRROR_H
#define MCLC_MIRROR_H

#include <string>

namespace MCLC {
    class Mirror {
    public:
        int index;
        static std::string replace(const std::string& origin_url) { return origin_url; };
        friend bool operator<(const Mirror& a, const Mirror& b) {
            return a.index < b.index;
        }
    };
}

#endif //MCLC_MIRROR_H