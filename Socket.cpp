#include "Socket.h"

Socket::Socket(char *host_name, char *port_number) {
    setup_socket(host_name, port_number);
}

Socket::Socket(int socket_fd) {
    this->socket_fd = socket_fd;
}

Socket::~Socket() {
    shutdown_socket();
}

ssize_t Socket::send_message(const void *message, size_t length, int flags) {
    return send(socket_fd, message, length, flags);
}

ssize_t Socket::recieve_message(void *buffer, size_t length, int flags) {
    return recv(socket_fd, buffer, length, flags);
}

int Socket::shutdown_socket() {
    return shutdown(socket_fd, SHUT_RDWR);
}

int Socket::get_socket_fd() {
    return socket_fd;
}

void Socket::setup_socket(char *host_name, char *port_number) {
    struct addrinfo hints, *service_info, *itr;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(host_name, port_number, &hints, &service_info);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo: %s.\n", gai_strerror(status));
        exit(1);
    }

    for (itr = service_info ; itr != NULL ; itr = itr->ai_next) {
        socket_fd = socket(itr->ai_family, itr->ai_socktype, itr->ai_protocol);
        if (socket_fd == -1) {
            perror("Error creating client socket ");
            continue;
        }

        if (connect(socket_fd, itr->ai_addr, itr->ai_addrlen) == -1) {
            shutdown_socket();
            perror("Error connecting client socket ");
            continue;
        }

        break;
    }

    freeaddrinfo(service_info);

    if (itr == NULL) {
        shutdown_socket();
        fprintf(stderr, "Failed to connect client socket to the specified address.\n");
        exit(1);
    }
}