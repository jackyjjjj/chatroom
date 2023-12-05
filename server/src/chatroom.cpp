#include "../include/chatroom.h"
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>

ChatRoom::ChatRoom(int port) : port(port), serverSocket(-1), epollFd(-1) {}

void ChatRoom::run()
{
    if (initialize())
    {
        epollLoop();
    }
}

void ChatRoom::stop()
{
    std::cout << "Stopping the chat room..." << std::endl;

    // Close the server socket
    if (serverSocket != -1)
    {
        close(serverSocket);
        serverSocket = -1;
    }

    // Close the epoll instance
    if (epollFd != -1)
    {
        close(epollFd);
        epollFd = -1;
    }

    // Close all client sockets
    for (const auto &user : users)
    {
        close(user.first);
    }

    // Clear the user list
    users.clear();

    std::cout << "Chat room stopped." << std::endl;
}

bool ChatRoom::initialize()
{
    // 创建监听套接字
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1)
    {
        perror("Error creating socket");
        return false;
    }

    // 设置套接字选项
    // 允许地址重用
    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        perror("Error setting socket options");
        close(serverSocket);
        return false;
    }

    // 绑定地址
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, (struct sockaddr *)(&serverAddr), sizeof(serverAddr)) == -1)
    {
        perror("Error binding socket");
        close(serverSocket);
        return false;
    }

    // 开始监听
    if (listen(serverSocket, 128) == -1)
    {
        perror("Error listening on socket");
        close(serverSocket);
        return false;
    }

    // 创建 epoll 实例
    epollFd = epoll_create1(0);
    if (epollFd == -1)
    {
        perror("Error creating epoll instance");
        close(serverSocket);
        return false;
    }

    // 添加监听套接字到 epoll
    epoll_event event{};
    event.events = EPOLLIN | EPOLLET;
    event.data.fd = serverSocket;
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, serverSocket, &event) == -1)
    {
        perror("Error adding server socket to epoll");
        close(serverSocket);
        close(epollFd);
        return false;
    }

    std::cout << "Server listening on port " << port << std::endl;

    return true;
}

void ChatRoom::epollLoop()
{
    constexpr int MAX_EVENTS = 100;
    epoll_event events[MAX_EVENTS];

    while (true)
    {
        int numEvents = epoll_wait(epollFd, events, MAX_EVENTS, -1);
        if (numEvents == -1)
        {
            perror("Error in epoll_wait");
            break;
        }

        for (int i = 0; i < numEvents; ++i)
        {
            if (events[i].data.fd == serverSocket)
            {
                // 新连接
                handleNewConnection();
            }
            else
            {
                // 处理客户端消息
                handleMessage(events[i].data.fd);
            }
        }
    }

    close(serverSocket);
    close(epollFd);
}

int ChatRoom::getPort()
{
    return port;
}

void ChatRoom::handleNewConnection()
{
    sockaddr_in clientAddr{};
    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientSocket = accept(serverSocket, reinterpret_cast<sockaddr *>(&clientAddr), &clientAddrLen);

    if (clientSocket == -1)
    {
        perror("Error accepting connection");
        return;
    }

    // 添加新连接到 epoll
    epoll_event event{};
    event.events = EPOLLIN | EPOLLET;
    event.data.fd = clientSocket;
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientSocket, &event) == -1)
    {
        perror("Error adding client socket to epoll");
        close(clientSocket);
        return;
    }

    // 向聊天室中的其他用户广播新用户加入
    broadcastMessage("User " + std::to_string(clientSocket) + " joined the chat.");

    std::cout << "New connection from " << inet_ntoa(clientAddr.sin_addr) << std::endl;
    users[clientSocket] = std::to_string(clientSocket);
}

void ChatRoom::handleMessage(int clientSocket)
{
    char buffer[4096];
    ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

    if (bytesRead <= 0)
    {
        // 客户端断开连接
        handleCloseConnection(clientSocket);
    }
    else
    {
        // 处理收到的消息
        buffer[bytesRead] = '\0';
        std::string message = users[clientSocket] + ": " + buffer;

        // 广播消息给其他用户
        broadcastMessage(message);
    }
}

void ChatRoom::handleCloseConnection(int clientSocket)
{
    std::cout << "User " << users[clientSocket] << " disconnected." << std::endl;

    // 从 epoll 中删除客户端套接字
    epoll_event event{};
    if (epoll_ctl(epollFd, EPOLL_CTL_DEL, clientSocket, &event) == -1)
    {
        perror("Error removing client socket from epoll");
    }

    // 从用户列表中删除用户
    users.erase(clientSocket);

    // 广播用户离开消息给其他用户
    broadcastMessage("User " + std::to_string(clientSocket) + " left the chat.");

    // 关闭客户端套接字
    close(clientSocket);
}

void ChatRoom::broadcastMessage(const std::string &message)
{
    for (const auto &user : users)
    {
        if (send(user.first, message.c_str(), message.size(), 0) == -1)
        {
            perror("Error sending message to user");
        }
    }
}