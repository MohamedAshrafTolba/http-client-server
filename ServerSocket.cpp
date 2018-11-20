#include "ServerSocket.h"
#include <cstring>

ServerSocket::ServerSocket(std::string &port_number, int backlog) {
    this->port_number = port_number;
    this->backlog = backlog;
    setup();
}

ServerSocket::~ServerSocket() {
    close();
}

int ServerSocket::accept_connection() {
    struct sockaddr client_address;
    socklen_t address_len = sizeof(client_address);
    return accept(socket_fd, &client_address, &address_len);
}

int ServerSocket::close() {
    return shutdown(socket_fd, SHUT_RDWR);
}

int ServerSocket::get_socket_fd() const {
    return socket_fd;
}

std::string ServerSocket::get_port_number() const {
    return port_number;
}

int ServerSocket::get_backlog() const {
    return backlog;
}

void ServerSocket::setup() {
    struct addrinfo hints, *service_info, *itr;
    int yes = 1;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int status = getaddrinfo(NULL, port_number.c_str(), &hints, &service_info);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo: %s.\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }

    for (itr = service_info ; itr != NULL ; itr = itr->ai_next) {
        socket_fd = socket(itr->ai_family, itr->ai_socktype, itr->ai_protocol);
        if (socket_fd == -1) {
            perror("Error creating server socket ");
            continue;
        }

        if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, 
                sizeof(int)) == -1) {
            perror("Error setting server socket options ");
            exit(EXIT_FAILURE);
        }

        if (bind(socket_fd, itr->ai_addr, itr->ai_addrlen) == -1) {
            close();
            perror("Error binding server socket to the specified port ");
            continue;
        }
        break;
    }

    freeaddrinfo(service_info);

    if (itr == NULL) {
        fprintf(stderr, "Failed to bind socket to port.\n");
        exit(EXIT_FAILURE);
    }
    
    if (listen(socket_fd, backlog) == -1) {
        perror("Error making socket listen to incoming connections ");
        exit(EXIT_FAILURE);
    }
}