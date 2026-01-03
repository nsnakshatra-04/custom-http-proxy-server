#include "blocklist.h"
#include <fstream>
#include <string>

bool is_blocked_domain(const std::string &host) {
    std::ifstream file("config/blocked_domains.txt");
    if (!file.is_open())
        return false;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#')
            continue;

        if (line == host)
            return true;
    }

    return false;
}
