#ifndef PROXY_FORWARD_H
#define PROXY_FORWARD_H

#include "http_parser.h"
#include <string>

bool forward_request(int client_fd, const HttpRequest &req);
bool tunnel_https(int client_fd, const std::string &host, int port);

#endif
