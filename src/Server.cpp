#include "Server.hpp"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <sstream>

Server::Server(int port) : port(port), server_fd(-1) {}

Server::~Server() {
    if (server_fd != -1) {
        close(server_fd);
    }
}

void Server::run() {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 1) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server listening on port " << port << "...\n";

    sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);

    int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
    if (client_fd < 0) {
        perror("accept failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "Client connected.\n";

    handleClient(client_fd);

    close(client_fd);
    std::cout << "Client disconnected.\n";
}

void Server::handleClient(int client_fd) {
    char buffer[1024];
    bool running = true;

    while (running) {
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
        if (bytes_read <= 0) {
            std::cout << "Client disconnected unexpectedly.\n";
            break;
        }

        std::string cmd(buffer);
        if (cmd.back() == '\n') cmd.pop_back();

        std::string response = processCommand(cmd);

        if (response == "SHUTDOWN") {
            std::string msg = "SHUTTING DOWN\n";
            send(client_fd, msg.c_str(), msg.size(), 0);
            running = false;
            break;
        }

        response += "\n";
        send(client_fd, response.c_str(), response.size(), 0);
    }
}

std::string Server::processCommand(const std::string &cmd) {
    std::istringstream iss(cmd);
    std::string action;
    iss >> action;

    if (action == "PUT") {
        std::string key, value;
        iss >> key >> value;
        store.put(key, value);
        return "OK";
    } else if (action == "GET") {
        std::string key;
        iss >> key;
        return store.get(key);
    } else if (action == "UPDATE") {
        std::string key, value;
        iss >> key >> value;
        store.update(key, value);
        return "OK";
    } else if (action == "DELETE") {
        std::string key;
        iss >> key;
        store.remove(key);
        return "OK";
    } else if (action == "GET_KEY") {
        std::string value;
        iss >> value;
        return store.getKey(value);
    } else if (action == "STATS") {
        return store.stats();
    } else if (action == "SHUTDOWN") {
        return "SHUTDOWN";
    }

    return "ERROR: Unknown command";
}
