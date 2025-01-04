#include <fstream>
#include <thread>
#include <vector>
#define CPPHTTPLIB_OPENSSL_SUPPORT

#include <exception>
#include <iostream>
#include <string>
#include <unistd.h>
#include <CLI/CLI.hpp>
#include "download/minecraft.h"
#include "download/mirror/mirrors.h"

void test();

int main(/*int argc, char* argv[]*/) {
    using namespace MCLC;
    Mirrors::load_mirror(Mirrors::MojangOfficial(), "MojangOfficial");
    //test();
    // Mirrors::loadMirror(Mirrors::BMCLAPI(), "BMCLAPI");

    // CLI::App app;
    // CLI11_PARSE(app, argc, argv);


     Downloader downloader = Downloader(64);
     std::string url = R"(https://wirelesscdn-download.xuexi.cn/publish/xuexi_android/latest/xuexi_android_10002068.apk)", path = "/mnt/c/Users/EncVar/CLionProjects/MCLC-2/cmake-build-debug/1.21.json/";
     downloader.process(url, path, true);
     try {
         downloader.start(std::cout, [](int percentage, int, int, int download_speed){
             //std::cout << percentage << "% " << download_speed << "\r";
             return false;
         });
     } catch (std::exception e) {
         std::cout << e.what();
     }
     downloader.join();
     downloader.merge();
    return 0;
}

void test() {
    using namespace std;
    MCLC::MinecraftDownloader downloader_client("", "");
    auto versions = downloader_client.get_versions();
    std::set <std::string> possible_native_strings;
    for(auto &str: versions) {
        std::cout << "[In]" << str.first << std::endl;
        auto version_json = downloader_client.get_version_json(str.first);
        auto new_native_predicate = [](std::string name) {
                int colons = 0;
                for(char &c: name)
                    if(c == ':')
                        colons++;
                return colons >= 3;
            };
        for(auto lib: version_json["libraries"]) {
            if(lib.contains("natives")) {
                for(auto [key, native_str]: lib["natives"].items())
                    possible_native_strings.insert(native_str);
            }
            if(new_native_predicate(std::string(lib["name"]))) {
                int last_column = std::string(lib["name"]).find_last_of(':');
                auto lib_name = std::string(lib["name"]);
                possible_native_strings.insert(lib_name.substr(last_column + 1, lib_name.length() - last_column - 1));
            }
        }
        std::cout << "[Out]" << str.first << std::endl;
        std::this_thread::sleep_for(1s);
    }
    for(auto &native_str: possible_native_strings)
        std::cout << native_str << std::endl;
    return;
}