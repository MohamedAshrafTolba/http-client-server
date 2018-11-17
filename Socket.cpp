#include "Socket.h"

Socket::Socket(std::string host_name, std::string post_number) {
    setup(port_number);
}

Socket::Socket(int socket_fd) {
    this->socket_fd = socket_fd;
}

Socket::~Socket() {
    shutdown();
}

ssize_t Socket::send(const void *message, size_t length, int flags) {
    return send(socket_fd, message, length, flags);
}

ssize_t Socket::recieve(void *buffer, size_t length, int flags) {
    return recieve(socket_fd, buffer, length, flags);
}

int Socket::shutdown() {
    return shutdown(socket_fd, SHUT_RDWR);
}

void Socket::setup(std::string host_name, std::string port_number) {
    struct addrinfo hints, *service_info, *itr;

    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(host_name.c_str(), port_number.c_str(), &hints, &service_info);
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
            shutdown();
            perror("Error connecting client socket ");
            continue;
        }

        break;
    }

    freeaddrinfo(service_info);

    if (itr == NULL) {
        fprintf(stderr, "Failed to connect client socket the specified address.\n");
        exit(1);
    }
}