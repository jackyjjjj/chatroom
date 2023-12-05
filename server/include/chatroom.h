#ifndef CHATROOM_H
#define CHATROOM_H

#include <iostream>
#include <vector>
#include <map>
#include <cstring>
#include <cstdlib>

using namespace std;

class ChatRoom
{
public:
    ChatRoom(int port);

    void run();
    void stop(); // New function to stop the chat room gracefully
    int getPort();

private:
    int port;
    int serverSocket;
    int epollFd;
    std::map<int, std::string> users;

    bool initialize();
    void epollLoop();
    void handleNewConnection();
    void handleMessage(int clientSocket);
    void handleCloseConnection(int clientSocket);
    void broadcastMessage(const std::string &message);
};

#endif // CHATROOM_H
