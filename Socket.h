#ifndef SOCKET_H
#define SOCKET_H

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

class Socket {
    public:
        Socket(char *host_name, char *port_number);
        Socket(int socket_fd);
        ~Socket();
        ssize_t send_message(const void *message, size_t length, int flags);
        ssize_t recieve_message(void *buffer, size_t length, int flags);
        int close();
        int get_socket_fd() const;
        char *get_host_name() const;
        char *get_port_name() const;
    private:
        int socket_fd;
        char *host_name;
        char *port_number;  
        void setup();
};

#endif