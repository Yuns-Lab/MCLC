#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <functional>
#include <fstream>
#include <iostream>
#include <atomic>
#include <filesystem>
#include <string>
#include "downloadexception.h"
#include "downloader.h"

namespace MCLC {
    //Downloader::ThreadPool
    void Downloader::ThreadPool::start(basic_download download_base, basic_callback callback, int thread_num) {
        const auto thread_task = [this](basic_download download_base, basic_callback callback) {
            mtx.lock();
            bool empty = scheduled_tasks.empty(); //Get task queue status
            mtx.unlock();
            bool last = false;
            while(!empty) {
                mtx.lock();
                DownloadTask task = scheduled_tasks.front(); //Fetch task from the front
                scheduled_tasks.pop();
                if(scheduled_tasks.empty())
                    last = true;
                mtx.unlock();
                download_base(task, callback);
                _finished.store(last);
                if(last)
                    break;
                mtx.lock();
                empty = scheduled_tasks.empty(); //Update the status
                mtx.unlock();
            }
        };
        std::cout << "?\n";
        while(thread_num--)
            threads.push_back(std::async(thread_task, download_base, callback));
        for(auto &v : threads)
            if(v.valid())
                v.wait();//Wait multi-thread tasks
    }

    void Downloader::ThreadPool::insert(const Downloader::DownloadTask &task) {
        scheduled_tasks.push(task); //Push new tasks to the end of the task queue
    }

    //Downloader
    void Downloader::process(const std::string& url, const std::string& path, bool cover) {
        using namespace std;
        const auto host_and_path = get_host_and_path(url);
        const auto filename = get_filename(url);
        std::cout << "Processing " << filename << "..." << endl;
        if(filesystem::exists(filesystem::path(path) / (filename))) {
            if(!cover)
                return;
            auto temp_path = filesystem::temp_directory_path() / ("mclc");
            if(!filesystem::exists(temp_path))
                filesystem::create_directories(temp_path);
            filesystem::copy_file(filesystem::path(path) / (filename),
                                  temp_path / filename, filesystem::copy_options::overwrite_existing);
            filesystem::remove(filesystem::path(path) / (filename)); //Store original file to temp directory
            covered.push_back(filesystem::path(path) / (filename));
        }
        cout << "Getting headers..." << endl;
        httplib::Client client(host_and_path.first);
        client.set_read_timeout(60);
        client.set_follow_location(true);
        //TODO: when there is no Content-Length header, put a warning in the log
        if(auto response = client.Head(host_and_path.second)) {
            if(response->status >= 400 || response->status <= 101) {
                throw DownloadException("Response status error: Status Code=" + to_string(response->status));
                return;
            } //If response isn't 2**(OK) or 3**(Redirect), throw an exception and then return
            filepaths.push_back(filesystem::path(path) / (filename));
            if(response->has_header("Content-Length")) {
                int content_size = atoi(response->get_header_value("Content-Length").c_str()); //Get content length of the file
                total_size += content_size;
                int i = 0;
                int temp_size = content_size;
                while(temp_size > 0) {
                    if(temp_size < (int) 1e5) { //Set the target download size of each thread to 100KB
                        tasks.emplace_back(url, path, filepaths.size() - 1, i, "bytes=" + to_string(int(i * 1e5)) + "-" + to_string(content_size - 1)), temp_size = 0;
                        break;
                    }
                    tasks.emplace_back(url, path, filepaths.size() - 1, i, "bytes=" + to_string(int(i * 1e5)) + "-" + to_string(int((i + 1) * 1e5 - 1)));
                    temp_size -= 1e5;
                    i++;
                }
            } else 
                tasks.emplace_back(url, path, filepaths.size() - 1); 
                //If the response doesn't have Content-Length header, add a task that downloads whole target file
        } else {
            throw DownloadException("Request failed. Please check your network connection and firewall settings.");
            return;
        } //Request failed
        cout << "Complete" << endl;
    }

    bool Downloader::start(std::ostream &out, basic_progress &&progress) {
        if(!manager.valid())
            return false;
        using namespace std;
        uint64_t previous = 0;
        cout << "Downloading..." << endl;
        mtx.unlock();

        int slept_time = 0;
        int download_speed = 0;

        while(manager.valid()) {
            this_thread::sleep_for(1ms);
            slept_time++; //Update slept time

            bool canceled = progress((double) downloaded_bytes.load() / total_size * 100, downloaded_bytes.load(), total_size, download_speed);
            if(canceled) {
                this -> cancel();
                return false; //Unfinished
            }

            if(slept_time % 500 == 0) //Calculate and update current download speed per 0.5 second
                download_speed = (downloaded_bytes.load() - previous) / long(5e5),
                previous = downloaded_bytes.load();
        }
        cout << "Downloaded" << endl;
        return true; //Finished
    }

    void Downloader::download(const DownloadTask& task, basic_callback callback) {
        const auto host_and_path = get_host_and_path(task.url); 
        httplib::Client client(host_and_path.first);

        //Set download ranges
        if(task.range != "full")
            client.set_default_headers({
                { "Range", task.range },
                { "Content-Type", "application/octet-stream" }
            });
        else {
            client.set_default_headers({
                { "Content-Type", "application/octet-stream" }
            });
        }
        std::atomic <uint64_t> downloaded_bytes = 0;
        httplib::Progress progress = [&](uint64_t current, uint64_t total) -> bool {
            bool cancelled = callback(current - downloaded_bytes.load());
            downloaded_bytes.store(current);
            return !cancelled;
        };
        //std::cout << "Downloading " << task.filename << '.' << task.index << "..." << std::endl;
        //Save temp chunks
        client.set_read_timeout(60);
        client.set_follow_location(true);
        if(const auto response1st = client.Get(host_and_path.second, progress)) {
            std::fstream output;
            const auto filepath = std::filesystem::path(task.path) / (task.filename + '.' + std::to_string(task.index) + ".mclcd");
            if(!std::filesystem::exists(task.path))
                std::filesystem::create_directories(task.path);
            if(std::filesystem::exists(filepath))
                std::filesystem::remove(filepath);
                
            output.open(filepath, std::ios::out);
            if(output.is_open())
                output << response1st->body;
            output.close();
        } else if(const auto response2nd = client.Get(host_and_path.second, progress)) {
            std::fstream output;
            const auto filepath = std::filesystem::path(task.path) / (task.filename + '.' + std::to_string(task.index) + ".mclcd");
            if(!std::filesystem::exists(task.path))
                std::filesystem::create_directories(task.path);
            if(std::filesystem::exists(filepath))
                std::filesystem::remove(filepath);

            output.open(filepath, std::ios::out);
            if(output.is_open())
                output << response2nd->body;
            output.close();
        } else if(const auto response3rd = client.Get(host_and_path.second, progress)) {
            std::fstream output;
            const auto filepath = std::filesystem::path(task.path) / (task.filename + '.' + std::to_string(task.index) + ".mclcd");
            if(!std::filesystem::exists(task.path))
                std::filesystem::create_directories(task.path);
            if(std::filesystem::exists(filepath))
                std::filesystem::remove(filepath);

            output.open(filepath, std::ios::out);
            if(output.is_open())
                output << response3rd->body;
            output.close();
        } else {
            throw DownloadException("Download failed. Please check your network connection and firewall settings.");
            return;
        } //Download failed
        //std::cout << "Downloaded " << task.filename << '.' << task.index << std::endl;
    }

    void Downloader::merge() {
        using namespace std;
        ofstream fout;
        ifstream fin;
        for(auto &v: tasks) {
            fin.open(std::filesystem::path(v.path) / (v.filename + '.' + std::to_string(v.index) + ".mclcd"), ios::in);
            fout.open(filepaths.at(v.file_index), ios::app | ios::out);
            stringstream buffer;
            buffer << fin.rdbuf();
            fin.close();
            while(!fout.is_open());
            fout.write(buffer.str().c_str(), buffer.str().length());
            fout.close();
            filesystem::remove(std::filesystem::path(v.path) / (v.filename + '.' + std::to_string(v.index) + ".mclcd"));
        } //Combine file chunks into target file
    }

    void Downloader::cancel() {
        using namespace std;
        cancelled.store(true);
        for(auto &v : tasks) //Remove all downloaded file chunks
            if(filesystem::exists(filesystem::path(v.path) / (v.filename + '.' + to_string(v.index) + ".mclcd")))
                filesystem::remove(filesystem::path(v.path) / (v.filename + '.' + to_string(v.index) + ".mclcd")); 
        for(auto &v : filepaths) //Remove all downloaded files
            if(filesystem::exists(v))
                filesystem::remove(v);
    }

    void Downloader::terminate() {
        using namespace std;
        const auto temp_path = filesystem::temp_directory_path() / ("mclc");
        if(cancelled.load()) { //Case cancelled
            int i = 0;
            for(auto &v : covered) {
                if(filesystem::exists(v))
                    filesystem::remove(v);
                filesystem::copy_file(filesystem::path(temp_path) / (to_string(i) + ".mclct"), v, filesystem::copy_options::overwrite_existing);
                i++;
            } //Restore all covered files
        } 
        for(int i = 0; i < covered.size(); i++) //Remove all temp files
            filesystem::remove(filesystem::path(temp_path) / (to_string(i) + ".mclct"));
    }

    Downloader::~Downloader() {
        terminate(); //Terminate the downloader when destructing
    }
}    // MCLC