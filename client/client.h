#ifndef CLIENT_H_
#define CLIENT_H_

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

using namespace std;

class ChatClient
{
public:
    ChatClient(const char *ip, int port);
    ~ChatClient();

    bool connectToServer();
    void run();

private:
    const char *serverIP;
    int serverPort;
    int clientSocket;
    pthread_t recvThread;

    static void *recvMessage(void *arg)
    {
        // 设置线程的取消状态为允许
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
        // 设置线程取消的类型为推迟取消
        pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
        ChatClient *client = (ChatClient *)arg;
        char buffer[4096] = {0};

        while (true)
        {
            pthread_testcancel();
            ssize_t len = recv(client->clientSocket, buffer, sizeof(buffer), 0);
            if (len <= 0)
            {
                break;
                // 服务器断开连接
            }
            buffer[len] = '\0';
            cout << buffer << endl;
        }
        client->disconnect();
        return NULL;
    }

    void sendMessage(const char *message);
    void disconnect();
};

#endif