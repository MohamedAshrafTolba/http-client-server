#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <vector>
#include <unistd.h>

#include "../ServerSocket.h"
#include "../Socket.h"
#include "../HttpWorkerThread.h"

#define TIMEOUT 30
#define SERVER_NAME "dummy"
#define HTTP_VERSION "HTTP/1.1"

class Server {
    public:
        Server(std::string &port_number, unsigned short backlog, unsigned long max_workers);
        ~Server();
        void run();
        std::string get_port_number() const;
        unsigned short get_backlog() const;
        unsigned long get_max_workers() const;
        int get_server_socket_fd() const;
    private:
        ServerSocket *server_socket;
        std::string port_number;
        unsigned short backlog;
        unsigned long max_workers;
        std::vector<HttpWorkerThread *> workers_pool;
};

#endif