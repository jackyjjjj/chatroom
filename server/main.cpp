#include "./include/server.h"
#include "./include/threadpool.h"
#include "./include/taskqueue.h"
#include "./include/chatroom.h"

#include <iostream>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVERPORT 9999

using namespace std;
Server *server;

void Unload()
{
    delete server;
}

int main()
{
    server = new Server(1, 5);
    atexit(Unload);
    server->start();

    // int sfd = socket(AF_INET, SOCK_STREAM, 0);
    // struct sockaddr_in saddr;
    // saddr.sin_family = AF_INET;
    // saddr.sin_port = htons(SERVERPORT);
    // saddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // if (bind(sfd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0)
    // {
    //     perror("bind()\n");
    //     exit(1);
    // }
    // if (listen(sfd, 5) < 0)
    // {
    //     perror("listen()\n");
    //     close(sfd);
    //     exit(1);
    // }
    while (1)
        ;
    exit(0);
}