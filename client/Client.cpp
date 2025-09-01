#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <string>

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket failed");
        return 1;
    }

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(4545);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address");
        return 1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        return 1;
    }

    std::cout << "Connected to server. Type commands (PUT/GET/UPDATE/DELETE/GET_KEY/STATS/SHUTDOWN).\n";

    char buffer[1024];
    bool running = true;

    while (running) {
        std::cout << "> ";
        std::string input;
        if (!std::getline(std::cin, input)) break;

        input += "\n";
        send(sock, input.c_str(), input.size(), 0);

        memset(buffer, 0, sizeof(buffer));
        ssize_t valread = read(sock, buffer, sizeof(buffer) - 1);
        if (valread <= 0) {
            std::cout << "Server closed connection.\n";
            break;
        }

        std::string response(buffer);
        if (response.find("SHUTTING DOWN") != std::string::npos) {
            std::cout << response;
            running = false;
        } else {
            std::cout << response;
        }
    }

    close(sock);
    return 0;
}
