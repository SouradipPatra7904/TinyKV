#ifndef SERVER_HPP
#define SERVER_HPP

#include "Store.hpp"
#include <string>

class Server {
public:
    Server(int port);
    ~Server();

    void run();

private:
    int server_fd;
    int port;
    Store store;

    void handleClient(int client_fd);
    std::string processCommand(const std::string &cmd);
};

#endif
