#ifndef SERVERSOCKET_H
#define SERVERSOCKET_H

#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

class ServerSocket {
public:
    ServerSocket(std::string port_number, unsigned short backlog);
    ~ServerSocket();
    int accept(struct sockadddr *address, socklen_t *address_len);
    int shutdown();
    int get_socket_fd();

private:
    int socket_fd;
    setup(std::string port_number, unsigned short backlog);
};

#endif