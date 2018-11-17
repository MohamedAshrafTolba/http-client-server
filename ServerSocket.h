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
    int shutdown_socket();
    int get_socket_fd();

private:
    int socket_fd;
    void setup_socket(char *port_number, int backlog);
};

#endif