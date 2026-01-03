#ifndef LOGGER_H
#define LOGGER_H

#include <string>

void log_request(const std::string &client_ip,
                 const std::string &host,
                 int port,
                 const std::string &action);

#endif
