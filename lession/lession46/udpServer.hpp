#pragma once

#include <iostream>
#include <string>
#include <functional>
#include <cstring>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

namespace Server
{
    const static std::string default_ip = "0.0.0.0";
    const static int gnum = 1024;
    using namespace std;

    enum
    {
        USAGE_ERROR = 1,
        SOCKET_ERROR,
        BIND_ERROR,
        OPEN_ERROR
        // LISTENQ = 1024
    };

    typedef function<void(int, string, uint16_t, string)> func_t;

    class udpServer
    {

    public:
        udpServer(const func_t &callback, const uint16_t &port, const std::string &ip = default_ip)
            : _callback(callback), _port(port), _ip(ip)
        {
        }

        void init()
        {
            // 1、创建socket
            _sockfd = socket(AF_INET, SOCK_DGRAM, 0);
            if (_sockfd == -1)
            {
                std::cerr << "socket error" << errno << ":" << strerror(errno) << std::endl;
                exit(SOCKET_ERROR);
            }
            std::cout << "socket suceess" << ":" << _sockfd << std::endl;

            // 2、绑定socket
            // 未来服务器需要明确的port，不能随便改变
            struct sockaddr_in local; // 定义了一个变量，栈上，用户态
            bzero(&local, sizeof(local));
            local.sin_family = AF_INET;
            local.sin_port = htons(_port);

            //_ip 是一个 std::string 类型，而 inet_addr() 这个函数要求的参数类型是 C 风格字符串（即 const char*），所以必须加上 .c_str() 来进行类型转换成C风格字符串
            // local.sin_addr.s_addr = inet_addr(_ip.c_str());//这个函数会把ip地址转换成一个32位的整数，然后再转成网络字节序
            local.sin_addr.s_addr = htonl(INADDR_ANY); // 任意地址绑定，服务器的真实写法
            int n = bind(_sockfd, (struct sockaddr *)&local, sizeof(local));
            if (n == -1)
            {
                std::cerr << "bind error" << errno << ":" << strerror(errno) << std::endl;
                exit(BIND_ERROR);
            }

            // UDP 服务器的预备工作完成
        }

        void start()
        {
            // 服务器的本质其实就是一个死循环
            char buf[gnum];
            for (;;)
            {
                // 读取数据
                struct sockaddr_in peer;
                socklen_t len = sizeof(peer);
                memset(buf, 0, sizeof(buf)); 
                ssize_t s = recvfrom(_sockfd, buf, sizeof(buf)-1, 0, (struct sockaddr *)&peer, &len);
                // 1 数据是什么  2 谁发的
                if (s > 0)
                {
                    buf[s] = '\0';
                    std::string clientIp = inet_ntoa(peer.sin_addr); // 1 网络序列需要转到主机序列  2 int-->点分十进制IP
                    uint16_t clientPort = ntohs(peer.sin_port);      // 网络序列转主机序列
                    std::string message = buf;
                    std::cout << clientIp << "[" << clientPort << "]" << " say: " << message << std::endl;

                    _callback(_sockfd, clientIp, clientPort, message);
                }
                else if (s == 0)
                {
                    std::cout << "[服务器 DEBUG] recvfrom 返回 0 (通常意味着对方发送了0字节数据包,或在某些TCP场景下表示关闭)" << std::endl; // 新增
                }
                else // s < 0, 表示出错
                {
                    // 使用 std::cerr 打印错误信息
                    std::cerr << "[服务器 DEBUG] recvfrom 出错! errno: " << errno << " (" << strerror(errno) << ")" << std::endl; // 新增
                    // 在某些严重错误后，可能需要考虑是否继续循环或退出
                }
            }
        }

        ~udpServer()
        {
        }

    private:
        uint16_t _port;
        std::string _ip; // 实际上，一款网络服务器，不建议指明一个IP
        int _sockfd;
        func_t _callback;
    };
} // namespace Server
