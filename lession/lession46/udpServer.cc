#include "udpServer.hpp"
#include <signal.h>
#include <memory>
#include <unordered_map>
#include <fstream>

using namespace std;
using namespace Server;


static void Usage(string proc)
{
    cout << "Usage: \n\t" << proc << " <port> " << endl;
}


// demo1
void handlerMessage(int sockfd, string ip, uint16_t port, string msg)
{
    string response_msg = msg;
    response_msg += "[server echo]";

    struct sockaddr_in client;
    bzero(&client, sizeof(client));

    client.sin_family = AF_INET;
    client.sin_port = htons(port);
    client.sin_addr.s_addr = inet_addr(ip.c_str());

    sendto(sockfd, response_msg.c_str(), response_msg.size(), 0, (struct sockaddr *)&client, sizeof(client));
}

// 执行服务器的时候，启动的时候 ./udpServer   port
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(USAGE_ERROR);
    }
    uint16_t port = atoi(argv[1]); // ASCII to Integer，把字符串转成整数

    std::unique_ptr<udpServer> usvr(new udpServer(handlerMessage, port));

    usvr->init();
    usvr->start();

    return 0;
}
