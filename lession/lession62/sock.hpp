#pragma once

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "log.hpp"

enum
{
    USAGE_ERR = 1,
    SOCKET_ERR,
    BIND_ERR,
    LISTEN_ERR
};
class Sock
{
    const static int backlog = 32;

public:
    static int Socket()
    {
        // 1、创建socket
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0)
        {
            logMessage(FATAL, "create socket error");
            exit(SOCKET_ERR);
        }
        logMessage(NORMAL, "create socket success: %d", sock);

        int opt = 1;
        setsockopt(sock,SOL_SOCKET,SO_REUSEADDR|SO_REUSEPORT,&opt,sizeof(opt));
        return sock;
    }

    static void Bind(int sock, int port)
    {
        // 2、绑定socket
        struct sockaddr_in local; // 定义了一个变量，栈上，用户态
        memset(&local, 0, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(port);
        local.sin_addr.s_addr = htonl(INADDR_ANY);
        if (bind(sock, (struct sockaddr *)&local, sizeof(local)) < 0)
        {
            logMessage(FATAL, "bind socket error");
            exit(BIND_ERR);
        }
        logMessage(NORMAL, "bind socket success");
    }

    static void Listen(int sock)
    {
        // 3. 设置sock为监听状态
        if (listen(sock, backlog) < 0)
        {
            logMessage(FATAL, "listen socket error");
            exit(LISTEN_ERR);
        }
        logMessage(NORMAL, "listen socket success");
    }

    static int Accept(int listensock, std::string *clientip, uint16_t *clientport)
    {
        struct sockaddr_in peer;
        socklen_t len = sizeof(peer);
        int sock = accept(listensock,(struct sockaddr *)&peer,&len);
        if(sock<0)
        {
            logMessage(ERROR,"accept error");

        }
        else
        {
        logMessage(NORMAL, "accept a new link success,get new sock :%d",sock);
        *clientip = inet_ntoa(peer.sin_addr);
        *clientport = ntohs(peer.sin_port);
        }
        return sock;
    }

};