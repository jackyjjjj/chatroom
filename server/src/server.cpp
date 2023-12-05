// Server.cpp

#include "../include/server.h"
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void func(void *arg)
{
    ChatRoom *room = (ChatRoom *)arg;
    room->run();
    return;
}

Server::Server(int minp, int maxp) : threadPool(minp, maxp) {}

Server::~Server()
{
    stop();
}

void Server::showrooms()
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9000);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0 || listen(fd, 10) < 0)
    {
        std::cout << "bind or listen error" << std::endl;
        return;
    }
    int cfd = accept(fd, NULL, NULL);
    string rooms = "rooms:";
    for (auto it : room_Queue)
    {
        rooms += to_string(it->getPort()) + ",";
    }
    write(cfd, rooms.c_str(), rooms.size());
    close(fd);
    close(cfd);
    return;
}

bool Server::addRoom(int port)
{
    ChatRoom *room = new ChatRoom(port);
    if (room == nullptr)
    {
        return false;
    }
    room_Queue.push_back(room);
    threadPool.addTask(Task(func, room));
    return true;
}

bool Server::removeRoom(int port)
{
    for (auto it = room_Queue.begin(); it != room_Queue.end(); it++)
    {
        if ((*it)->getPort() == port)
        {
            (*it)->stop();
            room_Queue.erase(it);
            return true;
        }
    }
    return false;
}

bool Server::start()
{
    if (initializeServer())
    {
        return true;
    }
    return false;
}

void Server::stop()
{
    cout << "server : call threadpool stop()" << endl;
    threadPool.stop();
}

bool Server::initializeServer()
{
    // 启动线程池
    if (!threadPool.start())
    {
        return false;
    }

    ChatRoom *room1 = new ChatRoom(10000);
    room_Queue.push_back(room1);
    threadPool.addTask(Task(func, room1));
    ChatRoom *room2 = new ChatRoom(9999);
    room_Queue.push_back(room2);
    threadPool.addTask(Task(func, room2));

    cout << "server:add task successfully" << endl;

    // 在这里可以添加其他初始化逻辑
    return true;
}
