#include <iostream>

#include "Server.h"

#define SERVER_NAME "127.0.0.1"
#define PORT_NUMBER "8080"
#define BACKLOG 10
#define MAX_WORKERS 10

int main(int argc, char **argv) {
    Server *server = new Server(SERVER_NAME, PORT_NUMBER, BACKLOG, MAX_WORKERS);
    
    std::cout << "The server has been successfully configured on port: " 
                    << server->get_port_number << std::endl;

    server->run();
}