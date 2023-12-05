#include "client.h"

ChatClient::ChatClient(const char *host, int port) : serverIP(host), serverPort(port), clientSocket(-1) {}

ChatClient::~ChatClient()
{
    disconnect();
}

bool ChatClient::connectToServer()
{
    // 创建套接字
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1)
    {
        perror("socket():创建套接字失败\n");
        return false;
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    if (inet_pton(AF_INET, serverIP, &serverAddr.sin_addr) <= 0)
    {
        perror("无效的地址或地址不受支持\n");
        close(clientSocket);
        return false;
    }

    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("connect():连接服务器失败\n");
        close(clientSocket);
        return false;
    }

    cout << "连接服务器成功" << endl;

    // 创建接受信息的线程
    if (pthread_create(&recvThread, NULL, &ChatClient::recvMessage, this) != 0)
    {
        perror("无法创建接收消息的线程\n");
        close(clientSocket);
        return false;
    }

    return true;
}

void ChatClient::run()
{
    char message[1024];
    while (true)
    {
        cout << "请输入消息(输入exit可退出):" << endl;
        cin.getline(message, sizeof(message));
        if (strcmp(message, "exit") == 0)
        {
            break;
        }
        sendMessage(message);
    }
    disconnect();
}

void ChatClient::sendMessage(const char *message)
{
    if (send(clientSocket, message, strlen(message), 0) < 0)
    {
        perror("发送消息失败\n");
    }
}

void ChatClient::disconnect()
{
    pthread_cancel(recvThread);
    if (pthread_join(recvThread, NULL) != 0)
    {
        perror("无法等待接收消息的线程结束\n");
    }
    if (clientSocket != -1)
    {
        close(clientSocket);
        cout << "已从服务器断开连接\n"
             << endl;
    }
}

int main()
{
    ChatClient client("127.0.0.1", 9999);
    if (client.connectToServer())
    {
        client.run();
    }
}