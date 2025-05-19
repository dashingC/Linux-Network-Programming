#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

//start
namespace client
{
    using namespace std;
    class udpClient
    {
    public:
        udpClient(const string &serverip, const uint16_t &serverport)
            : _serverip(serverip), _serverport(serverport), _sockfd(-1), _quit(false)
        {
        }
        void initClient()
        {
            // 1、创建socket
            _sockfd = socket(AF_INET, SOCK_DGRAM, 0);
            if (_sockfd == -1)
            {
                std::cerr << "socket error" << errno << ":" << strerror(errno) << std::endl;
                exit(2);
            }
            // client要不要bind[必须bind]，client要不要显式地bind，需不需要程序员自己bind？不需要

            cout << "socket suceess" << ":" << _sockfd << endl;
        }
        
        static void *readMessage(void *args)
        {
            int sockfd = *(static_cast<int *>(args));
            pthread_detach(pthread_self());

            while (true)
            {
                char buffer[1024];
                struct sockaddr_in tmp;
                socklen_t len = sizeof(tmp);
                size_t n = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&tmp, &len);

                if (n > 0)
                    buffer[n] = '\0';
                cout << "接收到的消息: " << buffer << endl;
            }
            return nullptr;
        }

        void run()
        {
            pthread_create(&_reader, nullptr, readMessage, (void *)&_sockfd);

            struct sockaddr_in server;
            memset(&server, 0, sizeof(server));
            server.sin_family = AF_INET;
            server.sin_addr.s_addr = inet_addr(_serverip.c_str());
            server.sin_port = htons(_serverport);

            string msg;
            char cmdline[1024];

            while (!_quit)
            {
                // cerr << "please enter msg:";
                // cin>>msg;
                fprintf(stderr,"Enter:");
                fflush(stderr);
                fgets(cmdline, sizeof cmdline, stdin);
                cmdline[strlen(cmdline)-1] = 0;
                msg = cmdline;

                sendto(_sockfd, msg.c_str(), msg.size(), 0, (struct sockaddr *)&server, sizeof(server));

                // char buffer[1024];
                // struct sockaddr_in tmp;
                // socklen_t len = sizeof(tmp);
                // size_t n = recvfrom(_sockfd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&tmp, &len);

                // if (n > 0)
                //     buffer[n] = 0;
                // cout << "执行结果:" << buffer << endl;
            }
        }
        ~udpClient()
        {
        }

    private:
        int _sockfd;
        string _serverip;
        uint16_t _serverport;
        bool _quit;
        pthread_t _reader;
        pthread_t _writer;
    };
}
