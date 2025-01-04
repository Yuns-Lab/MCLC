#include "config.h"
#include <fstream>

namespace MCLC {
    namespace Config {
        // basic_config configs = {
        //     {"concurrency", 64},
        //     {"", }
        // };

        void parse(std::filesystem::path _path) {
            using namespace std;
            using namespace nlohmann;
            ifstream fin(_path);
            json config_origin;
            if(fin.is_open())
                fin >> config_origin;
            //for(auto &v : config_origin)
                
        }
    }
} // MCLC