#include "mirrors.h"

namespace MCLC {
    namespace Mirrors {

        std::map<std::string, Mirror> mirror_registry;

        void load_mirror(const Mirror _mirror, const std::string& identifier) {
            mirror_registry.insert({identifier, _mirror});
        }

        std::string mirror(std::string origin_url) {
            return origin_url;
        }
    }
} // MCLC