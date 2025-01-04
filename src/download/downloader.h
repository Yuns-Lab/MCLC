#ifndef MCLC_DOWNLOADER_H
#define MCLC_DOWNLOADER_H

#include <vector>
#include <string>
#include <thread>
#include <iostream>
#include <queue>
#include <functional>
#include <mutex>
#include <atomic>
#include <filesystem>
#include <httplib.h>
#include "urlutil.h"

namespace MCLC {

    class Downloader {

    public:

        struct DownloadTask {
            std::string url, range, path, filename;
            int file_index, index;
            DownloadTask(const std::string& _url, const std::string& _path, int _file_index, const int& _index = -1, const std::string& _range = "full"):
                    url(_url), range(_range), path(_path), index(_index), file_index(_file_index) {
                filename = get_filename(url);
            }
        };


        typedef std::function <bool(uint64_t)> basic_callback;
        typedef std::function <void(const DownloadTask&, std::function<bool(uint64_t)>)> basic_download;
        typedef std::function <bool(int percentage, int downloaded, int total, int speed)> basic_progress;

        class ThreadPool {
            std::recursive_mutex mtx;
            std::queue <DownloadTask> scheduled_tasks;
            std::vector <std::thread> threads;
            std::atomic <bool> _finished;
        public:
            void insert(const DownloadTask &task);
            void start(basic_download, basic_callback, int thread_num);
            inline void lock() { mtx.lock(); }
            inline void unlock() { mtx.unlock(); }
            inline bool finished() { return _finished.load(); }
        };

    private:
        //Private Fields
        int concurrency;
        std::recursive_mutex mtx;
        std::vector <DownloadTask> tasks;
        std::vector <std::filesystem::path> filepaths;
        std::vector <std::filesystem::path> covered;
        std::atomic <bool> cancelled = false;
        std::atomic <uint64_t> downloaded_bytes = 0;
        static void download(const DownloadTask &, basic_callback callback);
        basic_callback progress_callback = [&](uint64_t _increased) {
            downloaded_bytes.fetch_add(_increased);
            return cancelled.load();
        };

        void manager_base(basic_download _func, basic_callback _callback) {
            mtx.lock();
            ThreadPool pool;
            mtx.unlock();
            for(auto &v : tasks)
                pool.insert(v);
            pool.start(_func, _callback, concurrency);
        }

        std::thread manager; //Create download manager thread
        uint64_t total_size = 0;

    public:

        explicit Downloader(const int &_concurrency): concurrency(_concurrency) {
            mtx.lock();
            manager = std::thread{&Downloader::manager_base, this, std::ref(Downloader::download), std::ref(Downloader::progress_callback)};
        }
        void process(const std::string&, const std::string&, bool);
        bool start(std::ostream&, std::function<bool(int, int, int, int)>);
        void join();
        void merge();
        void cancel();
        void terminate();
        virtual ~Downloader();
    };

} // MCLC

#endif //MCLC_DOWNLOADER_H
