#pragma once

#include <iostream>
#include "sock.hpp"

namespace select_ns
{
    static const int defaultport = 8080;
    class SlectServer
    {
    public:
    SlectServer(int port=defaultport):_port(port),_listensock(-1)
    {

    }

    void initServer()
    {
        _listensock = Sock::Socket();
        Sock::Bind(_listensock,_port);
        Sock::Listen(_listensock);
    }

    void start()
    {
        while(true)
        {
            std::string clientip;
            uint16_t clientport;
            int sock =Sock::Accept(_listensock,&clientip,&clientport);
            if(sock<0) continue;
        }
    }
    ~SlectServer()
    {
        if(_listensock<0)
        close(_listensock);
    }
    private:
        int _port;
        int _listensock;

    };
}