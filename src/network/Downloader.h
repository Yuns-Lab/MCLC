#ifndef MCLC_DOWNLOADER_H
#define MCLC_DOWNLOADER_H

#include <QObject>
#include <QList>
#include <QFile>
#include <QDir>
#include <QSslSocket>
#include <QtConcurrent>
#include <QHttpHeaders>
#include <QNetworkProxy>
#include <QNetworkAccessManager>

#include "HttpUtil.h"

namespace MCLC {

    struct DownloadTask {
        QUrl url;
        QList<QString> paths;

        DownloadTask(QUrl &_url, QList<QString> &&_paths = {}): url(_url), paths(_paths) {}

        DownloadTask &insertPath(QString &path) {
            paths.append(QDir::cleanPath(path));
            return (*this);
        }
    };

    class DownloadThreadTask : public QRunnable, QObject {
        int taskIndex = -1;
        int rangeBegin = -1, rangeEnd = -1;
        QNetworkAccessManager *parentManager;
        DownloadThreadTask(QNetworkAccessManager *manager, int index, int begin = -1, int end = -1) :
            taskIndex(index), rangeBegin(begin), rangeEnd(end), parentManager(manager) {}
        void run() override {
            QNetworkRequest request;
        }
    };

    struct DownloaderStatus {
        QString status;
        QString message;
        DownloaderStatus(QString &_status, QString &_message): status(_status), message(_message) {}
    };

    class Downloader : public QObject {
    private:

        QThreadPool *threadPool;
        QQueue<DownloadThreadTask> threadTasks;
        QQueue<DownloadThreadTask> retryThreadTasks;
        QNetworkAccessManager *manager;
        int concurrency = 64;
    public:
        Downloader(QList<DownloadTask> &tasks) {
            manager = new QNetworkAccessManager(this);
            threadPool = new QThreadPool(this);

        }
        inline Downloader &setProxy(QNetworkProxy &proxy) { manager->setProxy(proxy); return (*this); }
        inline Downloader &concurrent(int n) { this-> concurrency = n; return (*this); }
        inline Downloader &singleThread() { concurrency = 1; return (*this); }
        Downloader &start();
        void cancel();
        ~Downloader() override { delete manager; delete threadPool; }
    };

} // MCLC

#endif //MCLC_DOWNLOADER_H
