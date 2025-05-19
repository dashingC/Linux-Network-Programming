#include <iostream>
#include <cstdio>
#include <pthread.h>
#include <functional>
#include <cassert>
#include <string>
#include <cstring>
#include <unistd.h>
#include <memory>
#include "Thread.hpp" 

int tickets=1000;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

//会出现数据不一致的问题

void *getTicket(void* args)
{
    std::string username = static_cast<const char*>(args);
    while(true)
    {
        pthread_mutex_lock(&mutex);
        //临界区
        if(tickets>0)
        {
            std::cout<<username<<"正在出票....";
            tickets--;
            std::cout<<username<<"出票成功,剩余票数:"<<tickets<<std::endl;
            pthread_mutex_unlock(&mutex);
            usleep(1); 
        }
        else
        {
            std::cout<<username<<"出票完毕"<<std::endl;
            pthread_mutex_unlock(&mutex);
            break;
        }
    }
    return nullptr;
}

int main()
{
    std::unique_ptr<Thread> thread1( new Thread(getTicket,(void *)"user1",1));
    std::unique_ptr<Thread> thread2( new Thread(getTicket,(void *)"user2",2));
    std::unique_ptr<Thread> thread3( new Thread(getTicket,(void *)"user3",3));
    std::unique_ptr<Thread> thread4( new Thread(getTicket,(void *)"user4",4));


    thread1->join();
    thread2->join();
    thread3->join();
    thread4->join();



}