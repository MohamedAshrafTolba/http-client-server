#include "HttpWorkerThread.h"
#include <iostream>
using std::chrono::duration_cast;
using std::chrono::steady_clock;

HttpWorkerThread::HttpWorkerThread(int timeout) {
    this->timeout = timeout;
    start();
}

HttpWorkerThread::~HttpWorkerThread(){
    worker->join();
    delete worker;
}

bool HttpWorkerThread::is_done() {
    return done;
}

void HttpWorkerThread::start() {
    start_time = steady_clock::now();
    auto f = [&] () {
        int64_t prev_diff = 0;
        while (!done) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            std::cout << prev_diff << std::endl;
            auto now = steady_clock::now();
            int64_t time_diff = duration_cast<std::chrono::seconds>(now - start_time).count();
            prev_diff = time_diff;
            if (time_diff >= timeout) {
                done = true;
            }
        }    
    };
    worker = new std::thread(f);
}

int main() {
    HttpWorkerThread *t = new HttpWorkerThread(10);
    while (!t->is_done()) {
        std::cout << "Not Done Yet" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    std::cout << "Finally Done" << std::endl;
    delete t;
    std::cout << "Will Exit Now" << std::endl;
}
