#include "Client.h"
#include <sstream>
#include <fstream>
#include <iostream>

Client::Client(std::string &host, std::string &port,
    std::string &file_name, RequestMethod method, bool dry_run) {
    
    socket = new Socket(host, port);
    this->file_name = file_name;
    this->method = method;
    this->dry_run = dry_run;
}

Client::~Client() {
    if (socket != nullptr) {
        socket->close();
    }
    delete socket;
}

std::string Client::get_response() {
    this->make_request();
    std::string headers = socket->recieve_http_msg_headers();
    if (method != GET) {
        return headers;
    }
    HttpRequest request(headers); // Hacky implementation to parse the options
    std::string content_length_str = request.get_options()["Content-Length"];
    int content_length = atoi(content_length_str.c_str());
    std::string body = socket->recieve_http_msg_body(content_length);
    if (!dry_run) {
        write_file(file_name, body);
    }
    return headers + body;
}

int Client::get_client_socket_fd() const {
    return socket->get_socket_fd();
}

void Client::make_request() {
    std::stringstream request_stream;
    const static std::string req_method[] = {"GET", "POST", "INVALID"};
    request_stream << req_method[method] << ' ' << file_name << ' ' << HTTP_VERSION << "\r\n";
    if (method == POST) {
        std::string file_contents = read_file(file_name);
        request_stream << "Content-Length: " << file_contents.size() << "\r\n\r\n";
        request_stream << file_contents;
    } else {
        request_stream << "\r\n";
    }
    std::string request = request_stream.str();
    socket->send_http_msg(request);
}

std::string Client::read_file(std::string &file_name) {
    std::string file_path = "." + file_name;
    std::ifstream input_stream(file_path);
    std::string contents(std::istreambuf_iterator<char>(input_stream), {});
    input_stream.close();
    return contents;
}

void Client::write_file(std::string &file_name, std::string &contents) {
    std::string file_path = "." + file_name;
    std::ofstream output_stream(file_path);
    output_stream << contents;
    output_stream.close();
}