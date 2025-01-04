#ifndef MCLC_MINECRAFT_DOWNLOADER_H_
#define MCLC_MINECRAFT_DOWNLOADER_H_

#include <string>
#include <map>
#include <nlohmann/json.hpp>
#include "downloader.h"

namespace MCLC {
    class MinecraftDownloader {
    public:

        struct MCVersion {
            std::string version_number;
            int index;
            enum VersionType { RELEASE, SNAPSHOT, OLD_ALPHA, OLD_BETA };
            VersionType version_type;
            MCVersion() { index = -1; version_type = RELEASE; version_number = "unknown"; }
            MCVersion(const std::string& _version_number, const int& _index, const VersionType& _version_type): version_number(_version_number), index(_index), version_type(_version_type) {}
        };

        nlohmann::json get_version_manifest();
        std::map<std::string, MCVersion> get_versions();
        nlohmann::json get_version_json(const std::string&);
        void download_client(const std::string&);

        MinecraftDownloader(std::string _game_path, std::string _name, int _concurrency = 64): 
            version_manifest(load_version_manifest()), versions(load_versions()), game_path(_game_path), name(_name), concurrency(_concurrency) {}

    private:
        nlohmann::json version_manifest;
        std::map<std::string, MCVersion> versions;
        std::string game_path;
        std::string name;
        int concurrency;
        Downloader downloader = Downloader(concurrency);

        nlohmann::json load_version_manifest();
        std::map<std::string, MCVersion> load_versions();
        void download_assets(const nlohmann::json&);
        void download_libraries(const nlohmann::json&);
    };

} // MCLC

#endif //MCLC_MINECRAFT_H_
