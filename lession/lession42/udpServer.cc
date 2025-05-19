#include "udpServer.hpp"
#include "onlineUser.hpp"
#include <signal.h>
#include <memory>
#include <unordered_map>
#include <fstream>

using namespace std;
using namespace Server;

const std::string dictTxt = "./dict.txt";
unordered_map<string, string> dict;

static void Usage(string proc)
{
    cout << "Usage: \n\t" << proc << " <port> " << endl;
}

static bool cutString(const string &target, string *s1, string *s2, const string &sep)
{
    auto pos = target.find(sep);
    if (pos == string::npos)
        return false;
    *s1 = target.substr(0, pos);           //[)
    *s2 = target.substr(pos + sep.size()); //[)
    return true;
}

static void initDict()
{
    ifstream in(dictTxt, std::ios::binary);
    if (!in.is_open())
    {
        cerr << "open file" << dictTxt << " error" << endl;
        exit(OPEN_ERROR);
    }

    string line;
    string key, value;
    while (getline(in, line))
    {
        // cout<<line<<endl;
        if (cutString(line, &key, &value, ":"))
        {
            dict.insert(make_pair(key, value));
        }
    }

    in.close();

    cout << "load dict suceess:" << endl;
}

void reload(int signo)
{
    (void)signo;
    initDict();
}

static void debugPrint()
{
    for (auto &dt : dict)
    {
        cout << dt.first << " # " << dt.second << endl;
    }
}

// demo1
void handlerMessage(int sockfd, string ip, uint16_t port, string msg)
{
    // 可以对接收地数据进行特定的处理，而不用关心数据是怎么来的 ----server和业务逻辑解耦
    string response_msg;
    auto iter = dict.find(msg);
    if (iter == dict.end())
        response_msg = "not found";
    else
    {
        response_msg = iter->second;
    }

    // 返回去
    struct sockaddr_in client;
    bzero(&client, sizeof(client));

    client.sin_family = AF_INET;
    client.sin_port = htons(port);
    client.sin_addr.s_addr = inet_addr(ip.c_str());

    sendto(sockfd, response_msg.c_str(), response_msg.size(), 0, (struct sockaddr *)&client, sizeof(client));
}
// demo2
void execCommand(int sockfd, string ip, uint16_t port, string cmd)
{
    // 1、cmd解析
    // 2、如果必要，可能需要fork
    string response_msg;

    if (cmd.find("rm") != string::npos || cmd.find("mv") != string::npos || cmd.find("chmod") != string::npos || cmd.find("rmdir") != string::npos)
    {
        cerr << ip << ":" << port << "正在做一个非法的操作" << cmd << endl;
        response_msg = " 您的命令是一个非法操作";
        return;
    }

    FILE *fp = popen(cmd.c_str(), "r");
    if (fp == nullptr)
        response_msg = cmd + "exec error";

    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), fp) != nullptr)
    {
        response_msg += buffer;
    }
    pclose(fp);

    // 返回去
    struct sockaddr_in client;
    bzero(&client, sizeof(client));

    client.sin_family = AF_INET;
    client.sin_port = htons(port);
    client.sin_addr.s_addr = inet_addr(ip.c_str());

    sendto(sockfd, response_msg.c_str(), response_msg.size(), 0, (struct sockaddr *)&client, sizeof(client));
}
// demo3
OnlineUser onlineUser;
void routeMessage(int sockfd, string clientip, uint16_t clientport, string message)
{
    std::cout << "[Callback 开始] routeMessage - clientip: '" << clientip << "', clientport: " << clientport << ", message: '" << message << "'" << std::endl;
    if (message == "online") 
    {
        std::cout << "[Callback] 消息为 'online'" << std::endl;
        onlineUser.addUser(clientip, clientport); // onlineUser 是在 udpServer.cc 中定义的全局或静态变量
        std::cout << "[Callback] addUser 调用完毕" << std::endl;
    }
    if (message == "offline") 
    {
        std::cout << "[Callback] 消息为 'offline'" << std::endl;
        onlineUser.delUser(clientip, clientport); // onlineUser 是在 udpServer.cc 中定义的全局或静态变量
        std::cout << "[Callback] delUser 调用完毕" << std::endl;
    }

    std::cout << "[Callback] 准备检查用户是否在线..." << std::endl;
    bool is_online = onlineUser.isOnline(clientip, clientport); // onlineUser 是在 udpServer.cc 中定义的全局或静态变量
    std::cout << "[Callback] isOnline 返回: " << (is_online ? "是" : "否") << std::endl;
    

    if (is_online)
    {
        // 消息路由
        std::cout << "[Callback] 用户在线，准备广播消息..." << std::endl;
        onlineUser.broadcastMessage(sockfd,clientip,clientport,message);
        std::cout << "[Callback] broadcastMessage 调用完毕" << std::endl;
    }
    else
    {
        std::cout << "[Callback] 用户不在线，准备发送直接回复..." << std::endl;
        struct sockaddr_in client;
        bzero(&client, sizeof(client));

        client.sin_family = AF_INET;
        client.sin_port = htons(clientport);
        client.sin_addr.s_addr = inet_addr(clientip.c_str());

        string response_msg = "您还没有登录，请先登录,运行:online";
        
        sendto(sockfd, response_msg.c_str(), response_msg.size(), 0, (struct sockaddr *)&client, sizeof(client));
    }
}

// 执行服务器的时候，启动的时候 ./udpServer     port
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(USAGE_ERROR);
    }
    uint16_t port = atoi(argv[1]); // ASCII to Integer，把字符串转成整数
    // string ip = argv[2];

    // signal(2, reload);

    // initDict();
    // debugPrint();

    // std::unique_ptr<udpServer> usvr(new udpServer(handlerMessage, port));
    // std::unique_ptr<udpServer> usvr(new udpServer(execCommand, port));
    std::unique_ptr<udpServer> usvr(new udpServer(routeMessage, port));
    usvr->init();
    usvr->start();

    return 0;
}
