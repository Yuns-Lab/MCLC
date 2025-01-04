#include "minecraft.h"
#include "mirror/mirrors.h"
#include <httplib.h>
#include <nlohmann/json_fwd.hpp>
#include "downloadexception.h"
#include "urlutil.h"
#include "../utils/system.h"
#include <regex>

namespace MCLC {
    nlohmann::json MinecraftDownloader::load_version_manifest() {
        using namespace nlohmann;
        const auto url = Mirrors::mirror("https://piston-meta.mojang.com/mc/game/version_manifest_v2.json");
        const auto host_and_path = get_host_and_path(url);
        httplib::Client client(host_and_path.first);
        if(const auto response = client.Get(host_and_path.second))
            return json::parse(std::string(response->body));
        return nullptr;
    }

    std::map<std::string, MinecraftDownloader::MCVersion> MinecraftDownloader::load_versions() {
        using namespace nlohmann;
        const json versions = get_version_manifest();
        std::map<std::string, MCVersion> parsedVersions;
        int i = 0;
        auto transform = [](const std::string& version_type) {
            if(version_type == "release")
                return MCVersion::VersionType::RELEASE;
            if(version_type == "snapshot")
                return MCVersion::VersionType::SNAPSHOT;
            if(version_type == "old_alpha")
                return MCVersion::VersionType::OLD_ALPHA;
            if(version_type == "old_beta")
                return MCVersion::VersionType::OLD_BETA;
            return MCVersion::VersionType::RELEASE;
        };
        for(json object : versions["versions"])
            parsedVersions[object["id"]] = MCVersion({ object["id"], i++, transform(object["type"]) });
        return parsedVersions;
    }

    nlohmann::json MinecraftDownloader::get_version_json(const std::string &version_number) {
        using namespace nlohmann;
        const json manifest = get_version_manifest();
        const std::map<std::string, MCVersion> versions = get_versions();
        const MCVersion& current_version = versions.at(version_number);
        if(current_version.version_number == "unknown")
            throw DownloadException("Can't find current version.");
        const auto host_and_path = get_host_and_path(manifest["versions"].at(current_version.index)["url"]);
        httplib::Client version_index_request(host_and_path.first);
        if(const auto response = version_index_request.Get(host_and_path.second))
            return json::parse(std::string(response->body));
        return nullptr;
    }

    void MinecraftDownloader::download_client(const std::string &version_number) {
        
    }

    void MinecraftDownloader::download_assets(const nlohmann::json &version_index) {

    }

    void MinecraftDownloader::download_libraries(const nlohmann::json &version_index) {
        using namespace std;
        string system = System::get_os(), arch = System::get_arch();
        string macos_version = System::get_macos_version();
        bool native_support = false;
        const auto rule_check = [&](nlohmann::json rules) -> bool {
            bool allowed = true;
            if(!rules.is_array())
                return false;
            for(auto v: rules) {
                bool action = v["action"] == "allow";
                if(!v.contains("os") && !v.contains("arch"))
                    allowed = action;
                else if(v.contains("os") && v["os"]["name"] == system) {
                    if(!v["os"].contains("version"))
                        allowed = action;
                    else {
                        string version_requirement = v["os"]["version"];
                        while(version_requirement.contains("\\\\"))
                            version_requirement.replace(version_requirement.find("\\\\"), 2, "\\");
                        regex version_matcher(version_requirement);
                        allowed = !(action ^ regex_match(version_requirement, version_matcher));
                    }
                }
                else 
                    allowed = action; 
            }
            return allowed;
        };
        for(auto lib: version_index["libraries"]) {
            auto new_native_predicate = [](string name) {
                int colons = 0;
                for(char &c: name)
                    if(c == ':')
                        colons++;
                return colons >= 3;
            };
            if(lib.contains("rules") && rule_check(lib["rules"])) {
                if(lib.contains("natives")) {
                    if(!lib["natives"].contains(system))
                        throw DownloadException("The current version doesn't support current system.");;
                    string native_string = lib["natives"][system];
                    if(native_string.contains("${arch}")) {
                        if(arch == "x86")
                            native_string.replace(native_string.find("${arch}"), 7, "32");
                        else if(arch == "")
                            native_string.replace(native_string.find("${arch}"), 7, "64");
                        else
                            throw DownloadException("The current version doesn't support arm64 architecture.");
                    }
                }
                if(new_native_predicate(lib["name"])) {
                    string native_string = "natives-" + system + '-' + arch;
                    if(string(lib["name"]).contains(native_string)) {
                        
                    }
                }
            }
            
        }
    }

    nlohmann::json MinecraftDownloader::get_version_manifest() {
        return version_manifest;
    }

    std::map<std::string, MinecraftDownloader::MCVersion> MinecraftDownloader::get_versions() {
        return versions;
    }
} // MCLC