#include "Server.hpp"
#include <iostream>
#include <sstream>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

#define BUFFER_SIZE 1024

Server::Server(int port) : port(port) {}

void Server::run() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket failed");
        return;
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        close(server_fd);
        return;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        return;
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        close(server_fd);
        return;
    }

    std::cout << "Server listening on port " << port << "...\n";

    while (true) {
        new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        if (new_socket < 0) {
            perror("accept failed");
            continue;
        }

        memset(buffer, 0, BUFFER_SIZE);
        int valread = read(new_socket, buffer, BUFFER_SIZE);
        if (valread > 0) {
            std::string response = handleCommand(buffer);
            send(new_socket, response.c_str(), response.size(), 0);

            if (response == "SHUTTING DOWN\n") {
                close(new_socket);
                break;
            }
        }
        close(new_socket);
    }

    close(server_fd);
}

std::string Server::handleCommand(const std::string &cmd) {
    std::istringstream iss(cmd);
    std::string action, key, value;
    iss >> action;

    if (action == "PUT") {
        iss >> key >> value;
        store.put(key, value);
        return "OK\n";
    } else if (action == "GET") {
        iss >> key;
        auto val = store.get(key);
        return val.has_value() ? val.value() + "\n" : "NOT_FOUND\n";
    } else if (action == "UPDATE") {
        iss >> key >> value;
        return store.update(key, value) ? "OK\n" : "NOT_FOUND\n";
    } else if (action == "DELETE") {
        iss >> key;
        return store.remove(key) ? "OK\n" : "NOT_FOUND\n";
    } else if (action == "GET_KEY") {
        iss >> value;
        auto keyOpt = store.getKey(value);
        return keyOpt.has_value() ? keyOpt.value() + "\n" : "NOT_FOUND\n";
    } else if (action == "STATS") {
        return "Number of keys: " + std::to_string(store.size()) + "\n";
    } else if (action == "SHUTDOWN") {
        return "SHUTTING DOWN\n";
    }
    return "ERROR: Unknown command\n";
}
