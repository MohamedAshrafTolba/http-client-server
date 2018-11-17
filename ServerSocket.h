#ifndef SERVERSOCKET_H
#define SERVERSOCKET_H

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

class ServerSocket {
    public:
        ServerSocket(char *port_number, int backlog);
        ~ServerSocket();
        int accept_connection(struct sockaddr *address, socklen_t *address_len);
        int close();
        int get_socket_fd() const;
        char *get_port_number() const;
        int get_backlog() const;
    private:
        int socket_fd;
        char *port_number;
        int backlog;
        void setup();
};

#endif