#ifndef MCLC_HTTPUTIL_H
#define MCLC_HTTPUTIL_H

#include <QNetworkAccessManager>

namespace MCLC {
    void setSSLConfig(QNetworkRequest &request);
    void setDefaultHeaders(QNetworkRequest &request);
}

#endif //MCLC_HTTPUTIL_H
