#include "ServerSocket.h"

ServerSocket::ServerSocket(std::string port_number, unsigned short backlog) {
    setup(port_number, backlog);
}

ServerSocket::~ServerSocket() {
    shutdown();
}

int ServerSocket::accept(struct sockadddr *address, socklen_t *address_len) {
    return accept(address, address_len);
}

int ServerSocket::shutdown() {
    return shutdown(socket_fd, SHUT_RDWR);
}

int ServerSocket::get_socket_fd() {
    return socket_fd;
}

void ServerSocket::setup(std::string port_number, unsigned short backlog) {
    struct addrinfo hints, *service_info, *itr;

    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int status = getaddrinfo(NULL, port_number.c_str(), &hints, &service_info);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo: %s.\n", gai_strerror(status));
        exit(1);
    }

    for (itr = service_info ; itr != NULL ; itr = itr->ai_next) {
        socket_fd = socket(itr->ai_family, itr->ai_socktype, itr->ai_protocol);
        if (socket_fd == -1) {
            perror("Error creating socket ");
            continue;
        }

        if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, 
                sizeof(int)) == -1) {
            perror("Error setting socket options ");
            exit(1);
        }

        if (bind(socket_fd, itr->ai_addr, itr->ai_addrlen) == -1) {
            shutdown();
            perror("Error binding socket to the specified port error ");
            continue;
        }

        break;
    }

    freeaddrinfo(service_info);

    if (itr == NULL) {
        fprintf(stderr, "Failed to bind socket to port.\n");
        exit(1);
    }

    if (listen(socket_fd, backlog) == -1) {
        perror("Error making socket listen to incoming connections ");
        exit(1);
    }
}