#ifndef SOCKET_H
#define SOCKET_H

#include <cstring>
#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

class Socket {
public:
    Socket(char host_name[], char port_number[]);
    Socket(int socket_fd);
    ~Socket();
    ssize_t send_message(const void *message, size_t length, int flags);
    ssize_t recieve_message(void *buffer, size_t length, int flags);
    int shutdown_socket();
    int get_socket_fd();

private:
    int socket_fd;
    void setup_socket(char host_name[], char port_number[]);
};

#endif