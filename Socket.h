#ifndef SOCKET_H
#define SOCKET_H

#include <string>
#include <stdio.h>
#include <sys/types.h>

class Socket {
public:
    Socket(std::string host_name, std::string port_number);
    Socket(int socket_fd);
    ~Socket();
    ssize_t send(const void *message, size_t length, int flags);
    ssize_t recieve(void *buffer, size_t length, int flags);
    int shutdown();
    int get_socket_fd();

private:
    int socket_fd;
    void setup(std::string host_name, std::string port_number);
};

#endif