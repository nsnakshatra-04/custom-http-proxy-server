#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H

#include <string>

struct HttpRequest {
    std::string method;
    std::string host;
    int port;
    std::string raw_request;
};

bool parse_http_request(int client_fd, HttpRequest &req);

#endif
