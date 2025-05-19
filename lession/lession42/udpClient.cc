#include "udpClient.hpp"
#include <memory>

using namespace client;


static void Usage(string proc)
{
    cerr << "Usage: \n\t" << proc << " <port> " << endl;
}

//   ./udpClient  Server_ip    Server_port
int main(int argc, char *argv[])
{
     if (argc != 3)
    {
        Usage(argv[0]);
        exit(1);
    }

    string Serverip = argv[1];
    uint16_t Serverport = atoi(argv[2]);


    unique_ptr<udpClient> ucli(new udpClient(Serverip,Serverport));
    ucli->initClient();
    ucli->run();
    return 0;
}