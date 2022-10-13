#include "server.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cassert>

#include <iostream>
#include <string>

#include "scan.h"

namespace Server {

int32_t sock(uint32_t port, int32_t qlen) {
    struct protoent *ppe;
    struct sockaddr_in sin;
    int type = SOCK_STREAM;
    memset(&sin, 0, sizeof(sin));
    //указываем тип адреса - IPv4
    sin.sin_family = AF_INET;
    //указываем, в качестве адреса, шаблон INADDR_ANY - все сетевые интерфейсы
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons((unsigned short)port);
    //преобразовываем имя транспортного протокола в номер протокола
    if ((ppe = getprotobyname("tcp")) == 0) {
        std::cerr << "Eroor: Can't determine a transport protocol\n";
        return -1;
    }
    int s = socket(PF_INET, type, ppe->p_proto);
    if (s < 0) {
        std::cerr << "Error: Can't create the socket\n";
        return -1;
    }
    if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        std::cerr << "Error: Can't bind the socket\n";
        return -1;
    }
    if (type == SOCK_STREAM && listen(s, qlen) < 0) {
        std::cerr << "Error: Can't listen to the socket\n";
        return -1;
    }
    return s;
}

Server::Server(uint32_t port, int32_t qlen) : socket(sock(port, qlen)) {
    if (socket >= 0) std::cout << "== Scan service is started ==\n";
}

Server::~Server() {
    if (socket >= 0) {
        close(socket);
    }
}

void Server::Listen() {
    char msg[10000];
    while (socket >= 0) {
        sockaddr_in remaddr;  //структура IP-адреса клиента
        uint32_t remaddrs = sizeof(remaddr);
        int csock = accept(socket, (struct sockaddr *)&remaddr, &remaddrs);  //принимаем входящее подключение, адрес клиента в remaddr
        if (csock < 0) {                                                    //проверяем результат
            std::cerr << "Error: Can't accept client connection";
        } else {
            std::lock_guard<std::mutex> lock(m);
            memset(&msg, 0, sizeof(msg));
            if (read(csock, &msg, sizeof(msg)) > 0) {
                std::cout << "Directory: " << msg << "\n";
                std::string ans = Scan::ScanDirectory(msg);
                if (write(csock, ans.c_str(), ans.size()) > 0) {
                    std::cerr << "Data is sent\n";
                } else {
                    std::cerr << "Can't send data to client\n";
                }
            }
            else {
                std::cerr << "Can't read data\n";
            }
            close(csock);  //закрываем сокет клиента
        }
    }
}

}  // namespace Server
