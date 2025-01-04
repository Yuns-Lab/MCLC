#include "urlutil.h"

namespace MCLC {
    std::string get_filename(const std::string& url) {
        int length = url.length();
        for(int i = length - 1; i >= 0; i--)
            if(url.at(i) == '/' && i != length - 1)
                return url.substr(i + 1, length - 1);
        return "";
    }

    std::pair<std::string, std::string> get_host_and_path(const std::string& url) {
        int searched_slash = 0;
        if(url.find("https://") || url.find("http://"))
            searched_slash = 2;

        int i = 0;
        while(i < url.length() && searched_slash >= 0) {
            if(url.at(i) == '/')
                searched_slash--;
            i++;
        }
        i--;
        return std::make_pair(url.substr(0, i), url.substr(i, url.length() - 1));
    }
}