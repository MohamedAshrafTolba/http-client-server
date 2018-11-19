#ifndef SERVERSOCKET_H
#define SERVERSOCKET_H

#include <string>
#include <sys/socket.h>
#include <netdb.h>

class ServerSocket {
    public:
        ServerSocket(std::string &port_number, int backlog);
        ~ServerSocket();
        int accept_connection();
        int close();
        int get_socket_fd() const;
        std::string get_port_number() const;
        int get_backlog() const;
    private:
        int socket_fd;
        std::string port_number;
        int backlog;
        void setup();
};

#endif