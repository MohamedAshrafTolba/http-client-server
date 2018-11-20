#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <vector>
#include <unistd.h>

#include "../ServerSocket.h"
#include "../Socket.h"
#include "../HttpWorkerThread.h";

#define TIMEOUT 30
#define SERVER_NAME "dummy"

class Server {
    public:
        Server(std::string &port_number, int backlog, int max_workers);
        ~Server();
        void run();
        std::string get_port_number() const;
        int get_backlog() const;
        int get_max_workers() const;
    private:
        ServerSocket *server_socket;
        std::string port_number;
        int backlog;
        int max_workers;
        std::vector<HttpWorkerThread *> workers_pool;
};

#endif