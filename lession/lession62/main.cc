#include"selectServer.hpp"
#include <memory>


using namespace std;
using namespace select_ns;

int main()
{
    unique_ptr<SlectServer> svr(new SlectServer());

    svr->initServer();

    svr->start();
    return 0;

}