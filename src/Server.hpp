#ifndef SERVER_HPP
#define SERVER_HPP

#include "Store.hpp"
#include <string>

class Server {
public:
    explicit Server(int port);
    void run();

private:
    int port;
    Store store;

    std::string handleCommand(const std::string &cmd);
};

#endif
