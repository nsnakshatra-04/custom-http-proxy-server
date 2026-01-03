#include "http_parser.h"
#include <sys/socket.h>
#include <unistd.h>
#include <sstream>
#include <string>

bool parse_http_request(int client_fd, HttpRequest &req) {
    char buffer[4096];
    std::string request;
    while (true) {
    ssize_t bytes = recv(client_fd, buffer, sizeof(buffer), 0);
    if (bytes <= 0) {
        return false;
    }
    request.append(buffer, bytes);
    if (request.find("\r\n\r\n") != std::string::npos) {
        break;
    }
    if (bytes < (ssize_t)sizeof(buffer)) {
        break;
    }
}
    req.raw_request = request;
    req.port = 80; 
    std::istringstream stream(request);
    std::string line;
    if (!std::getline(stream, line))
        return false;
    if (!line.empty() && line.back() == '\r')
        line.pop_back();
    std::istringstream line_stream(line);
    line_stream >> req.method;
    while (std::getline(stream, line)) {
        if (line == "\r" || line.empty())
            break;
        if (line.back() == '\r')
            line.pop_back();
        if (line.rfind("Host:", 0) == 0) {
            std::string host_value = line.substr(5);
            while (!host_value.empty() && host_value[0] == ' ')
                host_value.erase(0, 1);

            size_t colon = host_value.find(':');
            if (colon != std::string::npos) {
                req.host = host_value.substr(0, colon);
                req.port = std::stoi(host_value.substr(colon + 1));
            } else {
                req.host = host_value;
            }
        }
    }
    return !req.method.empty() && !req.host.empty();
}
