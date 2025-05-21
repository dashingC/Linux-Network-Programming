#include <iostream>
#include <string>
#include <cstring>
#include <WinSock2.h>
#pragma warning(disable:4996)

#pragma comment(lib, "ws2_32.lib")

using namespace std;

uint16_t server_port = 8080;
string server_ip = "82.156.135.229";

int main()
{
    WSAData wsd;

    if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
    {
        cout << "error" << endl;
        return 0;
    }
    else
    {
        cout << "WSAStartup sucess" << endl;
    }

    SOCKET csock = socket(AF_INET, SOCK_DGRAM, 0);
    if (csock == SOCKET_ERROR)
    {
        cout << "socket error" << endl;
    }
    else
    {
        cout << "socket success" << endl;

    }

    struct sockaddr_in serve;
    memset(&serve, 0, sizeof(serve));
    serve.sin_family = AF_INET;
    serve.sin_port = htons(server_port);
    serve.sin_addr.s_addr = inet_addr(server_ip.c_str());

    //发数据
#define MAXLINE 1024
    char inbuffer[MAXLINE];
    string line;
    while (true)
    {
        //发送逻辑
        cout << "please input: ";
        getline(cin, line);
        int n = sendto(csock, line.c_str(), line.size(), 0, (struct sockaddr*)&serve, sizeof(serve));
        if (n < 0)
        {
            cout << "sendto error" << endl;
            break;
        }
        cout << "发送了 " << n << " 字节" << endl;

        //接收数据
        struct sockaddr_in peer;
        int peerlen = sizeof(peer);
        inbuffer[0] = 0;
        n = recvfrom(csock, inbuffer, sizeof(inbuffer) - 1, 0, (struct sockaddr*)&peer, &peerlen);

        if (n > 0)
        {
            inbuffer[n] = '\0'; 
            cout << "sever 接收到的消息是: " << inbuffer << endl;
        }
        else if (n == 0)
        {
            // 对于UDP，n=0 通常意味着对方发送了一个0字节的数据包
            // 在面向连接的协议如TCP中，n=0 通常表示对方关闭了连接
            cout << "接收到0字节数据包，或连接已关闭 (UDP场景下不常见)。" << endl;
        }
        else // n < 0 (即 SOCKET_ERROR)
        {
            // recvfrom 失败
            int error_code = WSAGetLastError();
            cout << "recvfrom error. Error code: " << error_code << endl;
            // 在这里可以根据具体的 error_code 进行更细致的判断
            // 例如：WSAETIMEDOUT (10060) 表示超时
            if (error_code == WSAETIMEDOUT) {
                cout << "接收超时 (WSAETIMEDOUT)." << endl;
            }
            break; // 出错则跳出循环
        }


    }

    return 0;
}