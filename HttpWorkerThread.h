#ifndef HTTPWORKERTHREAD_H
#define HTTPWORKERTHREAD_H

#include <chrono>
#include <mutex>
#include <thread>
#include <string>

class HttpWorkerThread {
public: 
    HttpWorkerThread(int timeout = 30);
    ~HttpWorkerThread();
    bool is_done();
private:
    std::mutex *lock;
    std::chrono::steady_clock::time_point start_time;
    int timeout;
    bool done;
    std::thread *worker;
    void start();
    void handle_http_request(std::string &request);
    std::string get_content_type(std::string url);
};

#endif