#include "Server.hpp"
#include <iostream>
#include <sstream>
#include <cstring>

Server::Server(int port) : port(port) {
    startTime = std::chrono::steady_clock::now();
}

void Server::run() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server started on port " << port << std::endl;

    while (true) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        activeConnections++;

        char buffer[1024] = {0};
        int valread;
        while ((valread = read(new_socket, buffer, 1024)) > 0) {
            std::string command(buffer, valread);
            std::string response = handleCommand(command);
            requestCount++;

            if (command.find("SHUTDOWN") == 0) {
                send(new_socket, response.c_str(), response.size(), 0);
                close(new_socket);
                activeConnections--;
                return;
            }

            send(new_socket, response.c_str(), response.size(), 0);
            memset(buffer, 0, sizeof(buffer));
        }

        close(new_socket);
        activeConnections--;
    }
}

std::string Server::handleCommand(const std::string &command) {
    std::istringstream iss(command);
    std::string cmd, key, value;
    iss >> cmd;

    if (cmd == "PUT") {
        iss >> key >> value;
        store.put(key, value);
        return "OK\n";
    } else if (cmd == "GET") {
        iss >> key;
        return store.get(key) + "\n";
    } else if (cmd == "UPDATE") {
        iss >> key >> value;
        store.update(key, value);
        return "OK\n";
    } else if (cmd == "DELETE") {
        iss >> key;
        store.remove(key);
        return "OK\n";
    } else if (cmd == "GET_KEY") {
        iss >> value;
        return store.getKey(value) + "\n";
    } else if (cmd == "STATS") {
        std::ostringstream stats;
        stats << "==== Server Statistics ====\n";
        stats << "Total Keys: " << store.count_size() << "\n";
        stats << "Uptime (s): "
              << std::chrono::duration_cast<std::chrono::seconds>(
                     std::chrono::steady_clock::now() - startTime)
                     .count()
              << "\n";
        stats << "Total Requests: " << requestCount << "\n";
        stats << "Active Connections: " << activeConnections << "\n";
        stats << "===========================\n";
        return stats.str();
    } else if (cmd == "SHUTDOWN") {
        return "Server shutting down...\n";
    }

    return "Invalid Command\n";
}
