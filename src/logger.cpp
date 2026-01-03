#include "logger.h"
#include <fstream>
#include <ctime>
#include <iomanip>

void log_request(const std::string &client_ip,
                 const std::string &host,
                 int port,
                 const std::string &action) {
    std::ofstream log_file("logs/proxy.log", std::ios::app);
    if (!log_file.is_open())
        return;
    std::time_t now = std::time(nullptr);
    std::tm *tm_now = std::localtime(&now);
    log_file << std::put_time(tm_now, "%Y-%m-%d %H:%M:%S")
             << " | " << client_ip
             << " | " << host << ":" << port
             << " | " << action
             << "\n";
}
