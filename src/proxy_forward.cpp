#include "proxy_forward.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

bool forward_request(int client_fd, const HttpRequest &req) {
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;       
    hints.ai_socktype = SOCK_STREAM; 
    std::string port_str = std::to_string(req.port);
    if (getaddrinfo(req.host.c_str(), port_str.c_str(), &hints, &res) != 0) {
        perror("getaddrinfo failed");
        return false;
    }
    int server_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (server_fd < 0) {
        perror("socket to destination failed");
        freeaddrinfo(res);
        return false;
    }
    if (connect(server_fd, res->ai_addr, res->ai_addrlen) < 0) {
        perror("connect to destination failed");
        close(server_fd);
        freeaddrinfo(res);
        return false;
    }
    freeaddrinfo(res);
    ssize_t total_sent = 0;
    ssize_t to_send = req.raw_request.size();
    const char *data = req.raw_request.c_str();
    while (total_sent < to_send) {
        ssize_t sent = send(server_fd, data + total_sent, to_send - total_sent, 0);
        if (sent <= 0) {
            perror("send to destination failed");
            close(server_fd);
            return false;
        }
        total_sent += sent;
    }
    char buffer[4096];
    ssize_t bytes;
    while ((bytes = recv(server_fd, buffer, sizeof(buffer), 0)) > 0) {
        ssize_t total_forwarded = 0;
        while (total_forwarded < bytes) {
            ssize_t sent = send(client_fd,
                                buffer + total_forwarded,
                                bytes - total_forwarded,
                                0);
            if (sent <= 0) {
                perror("send to client failed");
                close(server_fd);
                return false;
            }
            total_forwarded += sent;
        }
    }
    close(server_fd);
    return true;
}

bool tunnel_https(int client_fd, const std::string &host, int port) {
    struct addrinfo hints{}, *res;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    std::string port_str = std::to_string(port);
    if (getaddrinfo(host.c_str(), port_str.c_str(), &hints, &res) != 0) {
        return false;
    }
    int server_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (server_fd < 0) {
        freeaddrinfo(res);
        return false;
    }
    if (connect(server_fd, res->ai_addr, res->ai_addrlen) < 0) {
        close(server_fd);
        freeaddrinfo(res);
        return false;
    }
    freeaddrinfo(res);
    const char *ok =
        "HTTP/1.1 200 Connection Established\r\n"
        "Connection: close\r\n\r\n";
    send(client_fd, ok, strlen(ok), 0);
    fd_set fds;
    char buffer[4096];
    while (true) {
        FD_ZERO(&fds);
        FD_SET(client_fd, &fds);
        FD_SET(server_fd, &fds);
        int max_fd = std::max(client_fd, server_fd) + 1;
        if (select(max_fd, &fds, nullptr, nullptr, nullptr) < 0) {
            break;
        }
        if (FD_ISSET(client_fd, &fds)) {
            ssize_t n = recv(client_fd, buffer, sizeof(buffer), 0);
            if (n <= 0) break;
            send(server_fd, buffer, n, 0);
        }
        if (FD_ISSET(server_fd, &fds)) {
            ssize_t n = recv(server_fd, buffer, sizeof(buffer), 0);
            if (n <= 0) break;
            send(client_fd, buffer, n, 0);
        }
    }
    close(server_fd);
    return true;
}
