#ifndef MCLC_MIRRORS_H_
#define MCLC_MIRRORS_H_

#include "mirror.h"
#include <map>

namespace MCLC {

    namespace Mirrors {

        extern std::map<std::string, Mirror> mirror_registry;

        void load_mirror(const Mirror _mirror, const std::string& identifier);

        std::string mirror(std::string origin_url);

        class MojangOfficial: public Mirror {
        public:
            MojangOfficial() { this->index = 0; }
            static std::string replace(const std::string& origin_url)  {
                return origin_url;
            }
        };
//        class BMCLAPI: public Mirror {
//        public:
//            std::string replace(const std::string& origin_url) override;
//        };
    };

} // MCLC

#endif //MCLC_MIRRORS_H_
