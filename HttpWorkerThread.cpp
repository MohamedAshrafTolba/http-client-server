#include "HttpWorkerThread.h"
#include "HttpRequest.h"
#include "strutil.h"
#include <iostream>
#include <sstream>
#include <boost/filesystem.hpp>

using std::chrono::duration_cast;
using std::chrono::steady_clock;
using boost::filesystem::exists;
using boost::filesystem::last_write_time;
using boost::filesystem::path;
using boost::filesystem::file_size;
using boost::filesystem::load_string_file;
using boost::filesystem::save_string_file;

HttpWorkerThread::HttpWorkerThread(int socket_fd, std::string &http_version, int timeout) {
    this->socket = new Socket(socket_fd);
    this->timeout = timeout;
    this->http_version = http_version;
    start();
}

HttpWorkerThread::~HttpWorkerThread(){
    if (worker != nullptr) {
        worker->join();
    }
    delete worker;
    if (socket != nullptr) {
        socket->close();
    } 
    delete socket;
}

bool HttpWorkerThread::is_done() {
    return done;
}

void HttpWorkerThread::start() {
    start_time = steady_clock::now();
    worker = new std::thread(&HttpWorkerThread::execute, this);
}

void HttpWorkerThread::execute() {
    while (!done) {
        std::string headers = socket->recieve_http_msg_headers();
        handle_http_request(headers);
        auto now = steady_clock::now();
        int64_t time_diff = duration_cast<std::chrono::seconds>(now - start_time).count();
        if (time_diff >= timeout) {
            done = true;
        }
    }    
}

void HttpWorkerThread::handle_http_request(std::string &http_request) {
    HttpRequest request(http_request);

    std::stringstream response_stream;
    std::string file_name = "." + request.get_file_url();
    path file_path = path(file_name);
    std::string response_code = "200 OK";

    if (request.get_request_method() == GET) {
        if (!exists(file_path)) {
            response_code = "404 Not Found";
        }
        response_stream << http_version << ' ' << response_code << "\r\n";
        time_t now;
        time(&now);
        response_stream << "Date: " << strutil::time_cstr(&now) << "\r\n";
        response_stream << "Server: " << "Dummy" << "\r\n";
        time_t last_modified = last_write_time(file_path);
        response_stream << "Last-Modified: " << strutil::time_cstr(&last_modified) << "\r\n";
        response_stream << "Content-Length: " << file_size(file_path) << "\r\n";
        response_stream << "Content-Type: " << get_content_type(file_name) << "\r\n";
        response_stream << "\r\n";
        
        if (exists(file_path)) {
            std::string data;
            load_string_file(file_path, data);
            response_stream << data;
        }
        std::string response = response_stream.str();
        socket->send_http_msg(response);
    } else if (request.get_request_method() == POST) {
        std::string length_str = request.get_options()["Content-Length"];
        int length = atoi(length_str.c_str());
        if (length <= 0) {
            response_stream << http_version << ' ' << "400 Bad Request" << "\r\n";
            std::string response = response_stream.str();
            socket->send_http_msg(response);
            
            return;
        } else {
            response_stream << http_version << ' ' << response_code << "\r\n";
            std::string response = response_stream.str();
            socket->send_http_msg(response);
            
            std::string body = socket->recieve_http_msg_body(length);
            save_string_file(file_path, body);
        }
    }
}

std::string HttpWorkerThread::get_content_type(std::string url) {
	std::string contentType;
    std::string extension = url.substr(url.find_last_of(".") + 1);

	if (strutil::iequals(extension, "html")
			== 0) {
		contentType = "text/html";
	} else if (strutil::iequals(extension, "txt")
			== 0) {
		contentType = "text/txt";
	} else if (strutil::iequals(extension, "css")
			== 0) {
		contentType = "text/css";
	} else if (strutil::iequals(extension, "js")
			== 0) {
		contentType = "text/javascript";
	}else if (strutil::iequals(extension, "png")
			== 0) {
		contentType = "image/png";
	} else if (strutil::iequals(extension, "jpg")
			== 0) {
		contentType = "image/jpg";
	} else if (strutil::iequals(extension, "jpeg")
			== 0) {
		contentType = "image/jpg";
	} else if (strutil::iequals(extension, "gif")
			== 0) {
		contentType = "image/gif";
	} else {
		contentType = "text/txt";
	}
	return contentType;
}

int main() {
    std::string req = "GeT /README.md HTTP/1.1 \r\nHost: net.tutsplus.com \r\nUser-Agent: Mozilla/5.0 (Windows; U; Windows NT 6.1; en-US; rv:1.9.1.5) Gecko/20091102 Firefox/3.5.5 (.NET CLR 3.5.30729) \r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8 \r\nAccept-Language: en-us,en;q=0.5 \r\nAccept-Encoding: gzip,deflate \r\nAccept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7 \r\nKeep-Alive: 300 \r\nConnection: keep-alive \r\nCookie: PHPSESSID=r2t5uvjq435r4q7ib3vtdjq120 \r\nPragma: no-cache \r\nCache-Control: no-cache \r\n \r\n this is a useless body";
    HttpRequest request(req);
    
    std::string http_version = "HTTP/1.1";

    std::stringstream response_stream;
    std::string file_name = "." + request.get_file_url();
    path file_path = path(file_name);
    std::string response_code = "200 OK";
    if (!exists(file_path)) {
        response_code = "404 Not Found";
    }
    response_stream << http_version << ' ' << response_code << "\r\n";
    time_t now;
    time(&now);
    response_stream << "Date: " << strutil::time_cstr(&now) << "\r\n";
    response_stream << "Server: " << "Dummy" << "\r\n";
    time_t last_modified = last_write_time(file_path);
    response_stream << "Last-Modified: " << strutil::time_cstr(&last_modified) << "\r\n";
    response_stream << "Content-Length: " << file_size(file_path) << "\r\n";
    response_stream << "Content-Type: " << "text/txt" << "\r\n";
    response_stream << "\r\n";
    if (exists(file_path)) {
        std::string data;
        load_string_file(file_path, data);
        response_stream << data;
    }
    std::cout << response_stream.str();
    response_stream.clear();


    //////////////////// POST
    req = "post /test.txt HTTP/1.1 \r\nHost: net.tutsplus.com \r\nUser-Agent: Mozilla/5.0 (Windows; U; Windows NT 6.1; en-US; rv:1.9.1.5) Gecko/20091102 Firefox/3.5.5 (.NET CLR 3.5.30729) \r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8 \r\nAccept-Language: en-us,en;q=0.5 \r\nAccept-Encoding: gzip,deflate \r\nAccept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7 \r\nKeep-Alive: 300 \r\nConnection: keep-alive \r\nCookie: PHPSESSID=r2t5uvjq435r4q7ib3vtdjq120 \r\nPragma: no-cache\r\nContent-Length: 23\r\nCache-Control: no-cache \r\n \r\nthis is a useless body";
    request = HttpRequest(req);
    file_name = "." + request.get_file_url();
    file_path = path(file_name);
    std::string length_str = request.get_options()["Content-Length"];
    int length = atoi(length_str.c_str());
    if (length <= 0) {
        response_code = "400 Bad Request";
    } else {
        // TODO: Read request body into request.body.
        save_string_file(file_path, request.get_body());
    }
    response_stream << http_version << ' ' << response_code << "\r\n";
    std::cout << response_stream.str();
    std::cout << "Finally Done" << std::endl;
    // delete t;
    std::cout << "Will Exit Now" << std::endl;
}
