#include "Downloader.h"

#include <string>
#include <QNetworkReply>

namespace MCLC::Downloader {

    //Define mutex
    QMutex mutex;

    void DownloadThreadTask::run() {
        //Load http client default config
        QNetworkRequest request = QNetworkRequest(downloadTask.url);
        setSSLConfig(request);
        setDefaultHeaders(request);

        //Set ranges
        QString rangeHeader;
        if(rangeBegin)
            rangeHeader.append(std::to_string(rangeBegin)).append('-');
        if(rangeEnd)
            rangeHeader.append(std::to_string(rangeEnd));
        if(!rangeHeader.isEmpty()) // Range header is set only with readable Content-Length header
            request.setRawHeader("Range", rangeHeader.toUtf8());

        QNetworkReply *reply = manager->get(request);
        QEventLoop awaiter;
        if(QFile::exists(downloadTask.paths.at(0)))
            QFile::remove(downloadTask.paths.at(0));
        QFile firstFile(downloadTask.paths.at(0));
        firstFile.open(QFile::OpenModeFlag::ReadWrite);
        bool randomAccess = true;
        if(rangeBegin)
            randomAccess = !firstFile.seek(rangeBegin);

        long long finished = 0;
        //Connect to signals
        connect(reply, &QNetworkReply::downloadProgress, [&finished, &firstFile, &reply, &randomAccess, this](long long downloaded, long long _total) {
            long long increased = downloaded - finished;
            mutex.tryLock(-1); //Wait until the mutex is unlocked
            emit downloadProgress(increased); //Emit signal
            if(randomAccess)
                firstFile.write(reply->readAll());
            mutex.unlock();
            return;
        });
        //Wait until download progress finishes
        connect(manager, &QNetworkAccessManager::finished, &awaiter, &QEventLoop::quit);
        awaiter.exec();
        if(!randomAccess) {
            mutex.tryLock(-1);
            QByteArray file = firstFile.readAll();
            if(rangeEnd && file.size() < rangeEnd + 1)
                file.resize(rangeEnd + 1),
                file.replace(rangeBegin, rangeEnd - rangeBegin + 1, reply->readAll());
            file.erase(file.begin() + rangeBegin, file.end());
            file.push_back(reply->readAll());
        }
        firstFile.close();
        int index = 0;
        for(QString &path: downloadTask.paths) {
            if(index == 0) {
                index++;
                continue;
            }
            if(!QFileInfo(path).absoluteDir().exists())
                QFileInfo(path).absoluteDir().mkpath(QFileInfo(path).absoluteDir().path());
            firstFile.copy(path);
        }
        return;
   }

    Downloader &Downloader::start() {

        return (*this);
    }
}