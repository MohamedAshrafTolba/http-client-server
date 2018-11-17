#include "Socket.h"

Socket::Socket(char *host_name, char *port_number) {
    this->host_name = host_name;
    this->port_number = port_number;
    setup();
}

Socket::Socket(int socket_fd) {
    this->socket_fd = socket_fd;
}

Socket::~Socket() {
    close();
}

ssize_t Socket::send_message(const void *message, size_t length, int flags) {
    return send(socket_fd, message, length, flags);
}

ssize_t Socket::recieve_message(void *buffer, size_t length, int flags) {
    return recv(socket_fd, buffer, length, flags);
}

int Socket::close() {
    return shutdown(socket_fd, SHUT_RDWR);
}

int Socket::get_socket_fd() const {
    return socket_fd;
}

char *Socket::get_host_name() const {
    return host_name;
}

char *Socket::get_port_name() const {
    return port_number;
}

void Socket::setup() {
    struct addrinfo hints, *service_info, *itr;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(host_name, port_number, &hints, &service_info);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo: %s.\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }

    for (itr = service_info ; itr != NULL ; itr = itr->ai_next) {
        socket_fd = socket(itr->ai_family, itr->ai_socktype, itr->ai_protocol);
        if (socket_fd == -1) {
            perror("Error creating client socket ");
            continue;
        }

        if (connect(socket_fd, itr->ai_addr, itr->ai_addrlen) == -1) {
            close();
            perror("Error connecting client socket ");
            continue;
        }

        break;
    }

    freeaddrinfo(service_info);

    if (itr == NULL) {
        close();
        fprintf(stderr, "Failed to connect client socket to the specified address.\n");
        exit(EXIT_FAILURE);
    }
}