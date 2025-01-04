#ifndef MCLC_CONFIG_H
#define MCLC_CONFIG_H

#include <string>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <vector>

namespace MCLC {
    namespace Config {
        using nlohmann::json;

        struct Configuration {
            int download_concurrency = 64; //The number of download threads
            int download_basic_size = 1e5; //The download size of a single thread
        };

        // using basic_config = std::vector<Configuration>;
        // using config_type = Configuration;
        // extern basic_config configs;

        void parse(std::filesystem::path _path);
        // void to_json(json& _json, const basic_config _config);
        // void from_json(const json&, basic_config& _config);
    }
} // MCLC

#endif //MCLC_CONFIG_H
