#include "Server.hpp"

int main() {
    Server server(4545);
    server.run();
    return 0;
}
