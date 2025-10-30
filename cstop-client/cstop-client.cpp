#include "client.h"
#include <iostream>

int main() {
    SimpleClient client("127.0.0.1", 5000, "my_secret_token_123");
    if (!client.connectAndAuthenticate()) {
        std::cerr << "Cannot connect or authenticate. Exiting.\n";
        return 1;
    }
    std::cout << "Connection successful. Please enter your command.\n";

    while (client.isConnected()) {
        std::cout << "> ";
        std::string cmd;
        if (!std::getline(std::cin, cmd)) break;
        if (cmd.empty()) continue;
        std::string resp = client.sendCommandAndGetResponse(cmd);
        if (resp.empty()) {
            std::cerr << "Connection lost or empty response.\n";
            break;
        }
        std::cout << resp << std::endl;
        if (cmd == "QUIT") break;
    }

    client.closeConnection();
    return 0;
}
