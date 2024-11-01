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
#include <QMutex>

#include "utils/HttpUtil.h"

namespace MCLC::Downloader {

        extern QMutex mutex;

        struct DownloadTask {
            QUrl url;
            QList<QString> paths;

            DownloadTask(QUrl &_url, QList<QString> &&_paths = {}): url(_url), paths(_paths) {}

            DownloadTask &insertPath(QString &path) {
                paths.append(QDir::cleanPath(path));
                return (*this);
            }
        };

        class DownloadThreadTask : public QRunnable, public QObject {
            DownloadTask downloadTask;
            int rangeBegin = -1, rangeEnd = -1;
            QNetworkAccessManager *manager = new QNetworkAccessManager(this);
        public:
            DownloadThreadTask(DownloadTask &task, int begin = -1, int end = -1) :
                    rangeBegin(begin), rangeEnd(end), downloadTask(task) {}
            void run() override;
        signals:
            void downloadProgress(long long increased);
        };

        struct DownloaderStatus {
            QString status;
            QString message;
            DownloaderStatus(QString &_status, QString &_message): status(_status), message(_message) {}
        };

        class Downloader : public QObject {
        private:
            long long finished = 0, total = 0;
            QThreadPool *threadPool;
            QQueue<DownloadThreadTask> threadTasks;
            QQueue<DownloadThreadTask> retryThreadTasks;
            QNetworkProxy proxy;
            int concurrency = 64;
        public:
            Downloader(QList<DownloadTask> &tasks) {
                threadPool = new QThreadPool(this);
            }
            inline Downloader &setProxy(QNetworkProxy &proxy) { this->proxy = proxy; return (*this); }
            inline Downloader &concurrent(int n) { this->concurrency = n; return (*this); }
            inline Downloader &singleThread() { concurrency = 1; return (*this); }
            inline Downloader &downloadProgress(long long &_downloaded, long long &_total) {
                _downloaded = finished, _total = total; return (*this);
            };
            Downloader &start();
            void cancel();
            ~Downloader() override { delete threadPool; }
        };
    }
// MCLC

#endif //MCLC_DOWNLOADER_H
