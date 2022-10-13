#include "client.h"
#include <iostream>

const uint32_t SERVER_PORT = 1231;
const std::string SERVER_IP = "127.0.0.1"; // localhost

int main(int argc, char *argv[]) {
    if (argc > 1) {
        Client::Client cl(SERVER_IP, SERVER_PORT);
        cl.Send(argv[1]);
        std::cout << cl.Read();
    } else {
        std::cerr << "Set a directory\n";
    }
    return 0;
}