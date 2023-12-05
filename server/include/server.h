// Server.h

#ifndef SERVER_H
#define SERVER_H

#include "threadpool.h"
#include "chatroom2.h"

class Server
{
public:
    Server(int minp, int maxp);
    ~Server();

    // 启动服务器
    bool start();

    // 停止服务器
    void stop();

    bool addRoom(int port);

    bool removeRoom(int port);

    void showrooms();

private:
    int current_port = 10000;      // 监听端口
    ThreadPool threadPool;         // 线程池
    vector<ChatRoom *> room_Queue; // 聊天室

    // 初始化服务器
    bool initializeServer();
};

#endif // SERVER_H
