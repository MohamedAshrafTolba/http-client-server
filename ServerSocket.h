#ifndef SERVERSOCKET_H
#define SERVERSOCKET_H

#include <cstring>
#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

class ServerSocket {
public:
    ServerSocket(char port_number[], unsigned short backlog);
    ~ServerSocket();
    int accept_connection(struct sockadddr *address, socklen_t *address_len);
    int shutdown_socket();
    int get_socket_fd();

private:
    int socket_fd;
    void setupـsocket(char port_number[], unsigned short backlog);
};

#endif