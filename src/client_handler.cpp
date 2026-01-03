#include "client_handler.h"
#include "http_parser.h"
#include "proxy_forward.h"
#include "blocklist.h"
#include "logger.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>

void handle_client(int client_fd) {
    HttpRequest req;
    if (!parse_http_request(client_fd, req)) {
        close(client_fd);
        return;
    }
    sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    getpeername(client_fd, (sockaddr*)&client_addr, &addr_len);
    std::string client_ip = inet_ntoa(client_addr.sin_addr);
    if (is_blocked_domain(req.host)) {
        const char *response =
            "HTTP/1.1 403 Forbidden\r\n"
            "Content-Length: 0\r\n"
            "Connection: close\r\n\r\n";

        send(client_fd, response, strlen(response), 0);
        log_request(client_ip, req.host, req.port, "BLOCKED");
        close(client_fd);
        return;
    }
    if (req.method == "CONNECT") {
        bool ok = tunnel_https(client_fd, req.host, req.port);
        log_request(client_ip, req.host, req.port,
                    ok ? "TUNNEL" : "TUNNEL_FAILED");
        close(client_fd);
        return;
    }
    bool ok = forward_request(client_fd, req);
    log_request(client_ip, req.host, req.port,
                ok ? "ALLOWED" : "FAILED");

    close(client_fd);
}
