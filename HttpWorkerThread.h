#ifndef HTTPWORKERTHREAD_H
#define HTTPWORKERTHREAD_H

#include <chrono>
#include <mutex>
#include <thread>
#include <string>
#include <atomic>
#include "Socket.h"

class HttpWorkerThread {
public: 
    HttpWorkerThread(int socket_fd, std::string &http_version, int timeout = 30);
    ~HttpWorkerThread();
    bool is_done();
private:
    Socket *socket;
    std::chrono::steady_clock::time_point start_time;
    int timeout;
    std::atomic<bool> done;
    std::thread *worker;
    std::string http_version;
    void start();
    void execute();
    void handle_http_request(std::string &request);
    std::string get_content_type(std::string url);
};

#endif
