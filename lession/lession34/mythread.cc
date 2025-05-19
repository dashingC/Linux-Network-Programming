#include <iostream>
#include <cstdio>
#include <vector>
#include <pthread.h>
#include <functional>
#include <cassert>
#include <string>
#include <cstring>
#include <unistd.h>
#include <memory>
#include "Thread.hpp"
#include "Mutex.hpp"

int tickets = 1000;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

class ThreadData
{
public:
    ThreadData(const std::string &threadname) : threadname_(threadname)
    {
        std::cout << threadname_ << "创建成功" << std::endl;
    }
    ~ThreadData() {}

public:
    std::string threadname_;
};

void *getTicket(void *args)
{
    // std::string username = static_cast<const char*>(args);
    ThreadData *td = static_cast<ThreadData *>(args);
    while (true)
    {

        // 加锁和解锁的过程是多个线程串行执行的，程序会变慢
        // 锁只规定互斥访问，没有规定必须让谁优先执行
        //  pthread_mutex_lock(td->mutex_p_);
        // 临界区
        {
            LockGuard lockguard(&lock);
            if (tickets > 0)
            {
                std::cout << td->threadname_ << "出票成功,剩余票数:" << tickets-1 << std::endl;
                tickets--;
                // pthread_mutex_unlock(td->mutex_p_);
            }
            else
            {
                // pthread_mutex_unlock(td->mutex_p_);
                break;
            }
        }
        usleep(4000);
    }
    return nullptr;
}

int main()
{
#define NUM 4
    // pthread_mutex_t lock;
    // 初始化互斥锁
    // pthread_mutex_init(&lock, nullptr);

    std::vector<pthread_t> tids(NUM);

    for (int i = 0; i < NUM; i++)
    {
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "thread%d", i + 1);
        ThreadData *td = new ThreadData(buffer);
        pthread_create(&tids[i], nullptr, getTicket, (void *)td);
    }

    for (const auto &tid : tids)
    {
        pthread_join(tid, nullptr);
    }

    // pthread_t t1,t2,t3,t4;
    // pthread_create(&t1, nullptr, getTicket, (void *)"thread1");
    // pthread_create(&t2, nullptr, getTicket, (void *)"thread2");
    // pthread_create(&t3, nullptr, getTicket, (void *)"thread3");
    // pthread_create(&t4, nullptr, getTicket, (void *)"thread4");
    // pthread_join(t1, nullptr);
    // pthread_join(t2, nullptr);
    // pthread_join(t3, nullptr);
    // pthread_join(t4, nullptr);

    // 销毁互斥锁
    // pthread_mutex_destroy(&lock);
}