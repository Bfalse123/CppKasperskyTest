#include "server.h"

//проверка, задан ли шаблон INADDR_NONE, который обозначает сразу все доступные сетевые интерфейсы
//на некоторых платформах, он может быть не задан.
#ifndef INADDR_NONE
#define INADDR_NONE 0xfffffffff
#endif

const uint32_t SERVER_PORT = 1231;

int main() {
    Server::Server s(SERVER_PORT, 5);
    s.Listen();
    return 0;
}