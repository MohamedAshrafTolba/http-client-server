#include <iostream>

#include "Server.h"

#define BACKLOG 1000
#define MAX_WORKERS 1000

int main(int argc, char *argv[]) {
    if (argc < 2) {
        perror("Please enter the port number");
        return 1;
    }
    std::string port_number(argv[1]);
    Server *server = new Server(port_number, BACKLOG, MAX_WORKERS);
    std::cout << "The server has been successfully configured on port: " 
            << server->get_port_number() << std::endl;
    server->run();
    delete(server);
    return 0;
}