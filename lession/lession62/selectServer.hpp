#pragma once

#include <iostream>
#include "sock.hpp"
#include <sys/select.h>

namespace select_ns
{
    static const int defaultport = 8080;
    static const int defaultfd = -1;
    class SlectServer
    {
    public:
        SlectServer(int port = defaultport) : _port(port), _listensock(-1), fdarray(nullptr)
        {
        }

        void Print()
        {
            for (int i = 0; i < FD_SETSIZE; i++)
            {
                if (fdarray[i]!= defaultfd)
                std::cout << "fdarray[" << i << "] = " << fdarray[i] << std::endl;
            }
        }
        void HanderEvent(fd_set &rfds)
        {
            // listensocket
            std::string clientip;
            uint16_t clientport = 0;
            int sock = Sock::Accept(_listensock, &clientip, &clientport);
            if (sock < 0)
                return;
            logMessage(NORMAL, "accept success[%s:%d]", clientip.c_str(), clientport);
            // 将新连接的套接字添加到文件描述符数组中
            int i = 0;
            for (; i < FD_SETSIZE; i++)
            {
                if (fdarray[i] != defaultfd)
                    continue; // 跳过已使用的文件描述符
                else
                    break;
            }
            if (i == FD_SETSIZE)
            {
                logMessage(WARNING, "fdarray is full, cannot accept new connection");
                close(sock); // 关闭新连接
            }
            else
            {
                fdarray[i] = sock; // 将新连接的套接字添加到数组中
                // logMessage(NORMAL, "add new fd[%d] to fdarray", sock);
            }
            Print();
        }

        void initServer()
        {
            _listensock = Sock::Socket();
            Sock::Bind(_listensock, _port);
            Sock::Listen(_listensock);
            fdarray = new int[FD_SETSIZE];
            for (int i = 0; i < FD_SETSIZE; i++)
            {
                fdarray[i] = defaultfd; // 初始化文件描述符数组
            }
            fdarray[0] = _listensock; // 将监听套接字放在数组的第一个位置
            logMessage(NORMAL, "Server is listening");
        }

        void start()
        {
            fd_set rfds;
            FD_ZERO(&rfds);
            int maxfd = _listensock; // 最大文件描述符
            for (int i = 0; i < FD_SETSIZE; i++)
            {
                if (fdarray[i] == defaultfd)
                    continue;              // 跳过未使用的文件描述符
                FD_SET(fdarray[i], &rfds); // 将合法fd添加到读文件描述集中
                if (fdarray[i] > maxfd)
                    maxfd = fdarray[i]; // 更新最大文件描述符
            }

            while (true)
            {
                struct timeval timeout = {1, 0}; // 设置超时时间为3秒0微秒

                int n = select(maxfd + 1, &rfds, nullptr, nullptr, nullptr);
                switch (n)
                {
                case -1:
                    logMessage(WARNING, "select error");
                    break;
                case 0:
                    // 超时
                    logMessage(NORMAL, "timeout ... ");
                    break;
                default:
                    logMessage(NORMAL, "get a new link ");
                    HanderEvent(rfds);
                    break;
                }
                // sleep(1); // 模拟处理其他任务
                // std::string clientip;
                // uint16_t clientport;
                // int sock =Sock::Accept(_listensock,&clientip,&clientport);
                // if(sock<0) continue;
            }
        }
        ~SlectServer()
        {
            if (_listensock < 0)
                close(_listensock);
            if (fdarray)
                delete[] fdarray;
        }

    private:
        int _port;
        int _listensock;
        int *fdarray;
    };
}