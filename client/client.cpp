#include "client.h"

#include <arpa/inet.h>
#include <limits.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>

namespace Client {
int connectsock(const std::string &host, uint32_t port) {
    struct hostent *phe;     //указатель на запись с информацией о хосте
    struct servent *pse;     //указатель на запись с информацией о службе
    struct protoent *ppe;    //указатель на запись с информацией о протоколе
    struct sockaddr_in sin;  //структура IP-адреса оконечной точки

    //обнуляем структуру адреса
    memset(&sin, 0, sizeof(sin));
    //указываем тип адреса (IPv4)
    sin.sin_family = AF_INET;
    //задаем порт
    sin.sin_port = htons(port);
    //преобразовываем имя хоста в IP-адрес, предусмотрев возможность представить его
    //в точечном десятичном формате
    if (phe = gethostbyname(host.c_str()))
        memcpy(&sin.sin_addr, phe->h_addr, phe->h_length);
    //преобразовываем имя транспортного протокола в номер протокола
    if ((ppe = getprotobyname("tcp")) == 0) {
        std::cerr << "Can't determine a transport protocol\n";
        return -1;
    }
    //используем имя протокола для определения типа сокета
    int type = SOCK_STREAM;
    //создание сокета
    int s = socket(PF_INET, type, ppe->p_proto);
    if (s < 0) {
        std::cerr << "Error: can't create the socket\n";  //в случае неудачи выводим сообщение ошибки
        return -1;
    }
    //попытка подключить сокет
    if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        std::cerr << "Error: can't connect to the server\n";  //в случае неудачи выводим сообщение ошибки
        return -1;
    }
    return s;
}

Client::Client(const std::string &host, uint32_t port) : socket(connectsock(host, port)) {}

Client::~Client() {
    if (socket >= 0) {
        close(socket);
    }
}

void Client::Send(const std::string &data) const {
    if (socket >= 0 && (write(socket, data.c_str(), data.size()) < 0)) {
        std::cerr << "Can't sent data to the server\n";
    }
}

std::string Client::Read() const {
    char msg[1'000'000];
    if (socket >= 0 && (read(socket, msg, sizeof(msg)) < 0)) {
        std::cerr << "Can't receive data from the server\n";
        return "";
    }
    return msg;
}
}  // namespace Client