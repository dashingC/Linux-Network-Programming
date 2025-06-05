#pragma once

#include <iostream>
#include "sock.hpp"
#include <sys/select.h>

namespace select_ns
{
    static const int defaultport = 8080;
    class SlectServer
    {
    public:
        SlectServer(int port = defaultport) : _port(port), _listensock(-1)
        {
        }
        void HanderEvent(fd_set &rfds)
        {
            //listensocket
            std::string clientip;
            uint16_t clientport=0;
            int sock = Sock::Accept(_listensock, &clientip, &clientport);
            if (sock < 0) return;
            logMessage(NORMAL, "accept success[%s:%d]", clientip.c_str(), clientport);
        }

        void initServer()
        {
            _listensock = Sock::Socket();
            Sock::Bind(_listensock, _port);
            Sock::Listen(_listensock);
            logMessage(NORMAL, "Server is listening");
        }

        void start()
        {
            fd_set rfds;
            FD_ZERO(&rfds);
            FD_SET(_listensock, &rfds); // 将监听套接字添加到读文件描述集中
            while (true)
            {
                struct timeval timeout = {1, 0}; // 设置超时时间为3秒0微秒

                int n = select(_listensock + 1, &rfds, nullptr, nullptr, nullptr);
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
                    // if (FD_ISSET(_listensock, &rfds)) {
                    //     // 有新连接到来
                    //     std::string clientip;
                    //     uint16_t clientport;
                    //     int sock = Sock::Accept(_listensock, &clientip, &clientport);
                    //     if (sock < 0) {
                    //         std::cerr << "Accept error" << std::endl;
                    //         continue;
                    //     }
                    //     std::cout << "New connection from " << clientip << ":" << clientport << std::endl;
                    //     // 处理新连接
                    //     // ...
                    // }
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
        }

    private:
        int _port;
        int _listensock;
    };
}