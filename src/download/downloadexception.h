#ifndef MCLC_DOWNLOADEXCEPTION_H
#define MCLC_DOWNLOADEXCEPTION_H

#include <exception>
#include <string>

namespace MCLC {

    class DownloadException : public std::exception {
    private:
        std::string error_message;

    public:
        DownloadException(const std::string& _error_message) : error_message(_error_message) {}

        const char * what() const noexcept override {
            return error_message.c_str();
        }
    };

} // MCLC

#endif //MCLC_DOWNLOADEXCEPTION_H
