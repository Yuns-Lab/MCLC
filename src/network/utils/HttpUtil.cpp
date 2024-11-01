#include "HttpUtil.h"

namespace MCLC {
    void setSSLConfig(QNetworkRequest &request) {
        QSslConfiguration config(QSslConfiguration::defaultConfiguration());
        config.setProtocol(QSsl::TlsV1_3);
        config.setPeerVerifyMode(QSslSocket::VerifyNone);
        request.setSslConfiguration(config);
    }

    void setDefaultHeaders(QNetworkRequest &request) {
        request.setHeader(QNetworkRequest::UserAgentHeader, "MCLC/0.0.0(EncVar/MCLC)");
    }
}