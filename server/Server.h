#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <vector>
#include <unistd.h>

#include "../ServerSocket.h"
#include "../Socket.h"
#include "../HttpWorkerThread.h";

class Server {
    public:
        Server(char *server_name, char *port_number, int backlog, int max_workers);
        ~Server();
        void run();
        char *get_server_name() const;
        char *get_port_number() const;
        int get_backlog() const;
        int get_max_workers() const;
    private:
        ServerSocket *server_socket;
        char *server_name;
        char *port_number;
        int backlog;
        int max_workers;
        std::vector<HttpWorkerThread *> workers_pool;
};

#endif