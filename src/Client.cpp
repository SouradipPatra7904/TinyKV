#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>

constexpr int PORT = 4545;
constexpr size_t BUFFER_SIZE = 1024;

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return EXIT_FAILURE;
    }

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        close(sock);
        return EXIT_FAILURE;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed");
        close(sock);
        return EXIT_FAILURE;
    }

    std::string cmd;
    char buffer[BUFFER_SIZE];

    while (true) {
        // Only print prompt if running interactively
        if (isatty(STDIN_FILENO)) {
            std::cout << "> ";
        }

        // Exit loop if no input
        if (!std::getline(std::cin, cmd)) break;

        // Send command to server
        send(sock, cmd.c_str(), cmd.size(), 0);

        // Read server response
        ssize_t valread = read(sock, buffer, BUFFER_SIZE - 1);
        if (valread <= 0) break;

        buffer[valread] = '\0';
        std::cout << buffer << std::endl;

        // Exit if shutdown command was sent
        if (cmd.rfind("SHUTDOWN", 0) == 0) break;

        // If running non-interactively (script), exit after one command
        if (!isatty(STDIN_FILENO)) break;
    }

    close(sock);
    return 0;
}
