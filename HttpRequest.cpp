#include "HttpRequest.h"
#include "strutil.h"
#include <iostream>
#include <string>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/trim.hpp>

using boost::algorithm::trim;

HttpRequest::HttpRequest(std::string &request) {
    parse(request);
}

HttpRequest::~HttpRequest() {

}

std::string HttpRequest::get_file_url() {
    return file_url;
}

std::unordered_map<std::string, std::string> &HttpRequest::get_options() {
    return options;
}

RequestMethod HttpRequest::get_request_method() {
    return method;
}

std::string HttpRequest::get_client_version() {
    return client_version;
}

std::string HttpRequest::get_body() {
    return body;
}

void HttpRequest::parse(std::string &request) {
    typedef boost::tokenizer<boost::char_separator<char> > 
    tokenizer;
    boost::char_separator<char> line_sep("\r\n");
    tokenizer lines(request, line_sep);
    int line_no = 0;
    for (std::string line : lines) {
        line_no++;
        trim(line);
        if (line_no == 1) {
            boost::char_separator<char> space_sep(" ");
            tokenizer fields(line, space_sep);
            int idx = 0;
            for (std::string field : fields) {
                idx++;
                switch(idx) {
                case 1:
                if (strutil::iequals(field, std::string("GET"))) {
                    method = GET;
                } else if (strutil::iequals(field, std::string("POST"))) {
                    method = POST;
                } else {
                    //ERROR
                }
                case 2:
                    file_url = field;
                case 3:
                    client_version = field;
                } 
            }
         } else {
            auto colon_idx = line.find_first_of(":");
            if (colon_idx != std::string::npos) {
                std::string name = line.substr(0, colon_idx);
                trim(name);
                std::string value = line.substr(colon_idx + 1);
                trim(value);   
                options[name] = value;
            } else if (line.length() != 0) {
                body += line + "\n";
            }
        }
    }
    if (body.length() > 0) {
        body = body.substr(0, body.length() - 1);
    }
}

int main() {
    std::string request = "GeT /tutorials/other/top-20-mysql-best-practices/ HTTP/1.1 \r\nHost: net.tutsplus.com \r\nUser-Agent: Mozilla/5.0 (Windows; U; Windows NT 6.1; en-US; rv:1.9.1.5) Gecko/20091102 Firefox/3.5.5 (.NET CLR 3.5.30729) \r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8 \r\nAccept-Language: en-us,en;q=0.5 \r\nAccept-Encoding: gzip,deflate \r\nAccept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7 \r\nKeep-Alive: 300 \r\nConnection: keep-alive \r\nCookie: PHPSESSID=r2t5uvjq435r4q7ib3vtdjq120 \r\nPragma: no-cache \r\nCache-Control: no-cache \r\n \r\n this is a useless body";
    HttpRequest http(request);
    std::cout << http.get_request_method() << std::endl;
    std::cout << http.get_file_url() << std::endl;
    std::cout << http.get_client_version() << std::endl;
    std::cout << http.get_body() << std::endl;
    for (auto entry : http.get_options()) {
        std::cout << entry.first << ": " << entry.second << std::endl;
    }
}
