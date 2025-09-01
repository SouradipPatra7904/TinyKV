#ifndef SERVER_HPP
#define SERVER_HPP

#include "Store.hpp"
#include <string>
#include <chrono>
#include <netinet/in.h>
#include <unistd.h>

class Server {
public:
    Server(int port);
    void run();

private:
    int port;
    Store store;
    std::chrono::steady_clock::time_point startTime;
    size_t requestCount = 0;
    size_t activeConnections = 0;

    std::string handleCommand(const std::string &command);
};

#endif
