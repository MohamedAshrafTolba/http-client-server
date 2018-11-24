#include "Client.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "../strutil.h"

Client::Client(std::string &host_name, std::string &port_number,
        std::string &requests_file, bool dry_run) {
    socket = new Socket(host_name, port_number);
    this->requests_file = requests_file;
    this->dry_run = dry_run;
}

Client::~Client() {
    if (socket != nullptr) {
        delete socket;
    }
}

void Client::run() {
    std::mutex pipelined_requests_mutex;
    std::condition_variable pipelined_requests_cond_var;

    auto f = [&] {
        while (true) {
            pipelined_requests_mutex.lock();
            pipelined_requests_cond_var.wait(pipelined_requests_mutex, [] {
                return !pipelined_requests.empty();
            });
            // todo pop from the queue and pass this file_name to get_response()
        }
    };

    std::thread pipelining_thread(f);

    std::ifstream requests_file_stream(requests_file);
    std::string request;
    while (std::getline(requests_file_stream, request)) {
        std::stringstream split_stream(request);
        std::string method, file_name;
        split_stream >> method;
        if (!split_stream.eof() && split_stream.tellg() != -1) {
            split_stream >> file_name;
        } else {
            perror("Skipping request: File name is missing");
            continue;
        }
        RequestMethod req_method = NOP;
        if (strutil::iequals(method, "GET")) {
            req_method = GET;
        } else if (strutil::iequals(method, "POST")) {
            req_method = POST;
        }
        make_request(req_method, file_name);
        if (req_method == POST) {
            get_response(); // todo check this function 
        } else {
            pipelined_requests_mutex.lock();
            pipelined_requests.push_back(file_name);
            pipelined_requests_mutex.unlock();
            pipelined_requests_cond_var.notify_one()
        }
    }

    pipelining_thread.join();
}

int Client::get_client_socket_fd() const {
    return socket->get_socket_fd();
}

std::string Client::get_requests_file() const {
    return requests_file;
}

bool Client::is_dry_run() const {
    return dry_run;
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

void Client::make_request(RequestMethod method, std::string &file_name) {
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

std::string Client::get_response() {
    // todo
}