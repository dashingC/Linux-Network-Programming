#include "RingQueue.hpp"
#include "Task.hpp"
#include <iostream>
#include <pthread.h>
#include <ctime>
#include <unistd.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

std::string SelfName()
{
    char name[128];
    snprintf(name,sizeof name,"thread[0x%x]",pthread_self());
    return name;
}

void *producer_routine(void *rq)
{
    // RingQueue<int> *ringqueue = static_cast<RingQueue<int> *>(rq);

    RingQueue<Task> *ringqueue = static_cast<RingQueue<Task> *>(rq);
    while (true)
    {
        //version 1
        // sleep(1);
        // int data = rand() % 10+1;
        // ringqueue->push(data);
        // pthread_mutex_lock(&lock);
        // std::cout << "producer: " << data << std::endl;
        // pthread_mutex_unlock(&lock);

        //version2
        //构建任务   这个是要花时间的
        int x= rand() % 50;
        int y= rand() % 50;
        char op = oper[rand() % oper.size()];
        Task task(x, y, op, mymath);
        //生产任务
        ringqueue->push(task);
        //输出提示
        pthread_mutex_lock(&lock);
        std::cout << SelfName() <<"生产者派发了一个任务: " << task.toTaskString() << std::endl;
        pthread_mutex_unlock(&lock);
        sleep(1);




    }
}

void *consumer_routine(void *rq)
{
    // RingQueue<int> *ringqueue = static_cast<RingQueue<int> *>(rq);

    RingQueue<Task> *ringqueue = static_cast<RingQueue<Task> *>(rq);
    while (true)
    {
        // //version 1
        // int data;
        // ringqueue->pop(&data);
        // pthread_mutex_lock(&lock);
        // std::cout << "consumer.......................: " << data << std::endl;
        // pthread_mutex_unlock(&lock);
        // // sleep(1);

        // version2
        Task task;
        //消费任务   这个也是要花时间的
        ringqueue->pop(&task);
        std::string result = task();
        pthread_mutex_lock(&lock);
        std::cout << SelfName()<< "消费者消费了一个任务:................. " << result << std::endl;
        pthread_mutex_unlock(&lock);
        // sleep(1);

    }
}

int main()
{
    // RingQueue<int> *rq = new RingQueue<int>();
    RingQueue<Task> *rq = new RingQueue<Task>();
    pthread_t p[4], c[8];

    for (int i = 0; i < 4; ++i)
    {
        pthread_create(p+i, nullptr, producer_routine, rq);
    }
    for (int i = 0; i < 8; ++i)
    {
        pthread_create(c+i, nullptr, consumer_routine, rq);
    }

    // pthread_create(&p, nullptr, producer_routine, rq);
    // pthread_create(&c, nullptr, consumer_routine, rq);

    for (int i = 0; i < 4; ++i)
    {
        pthread_join(p[i], nullptr);
    }

    for (int i = 0; i < 8; ++i)
    {
        pthread_join(c[i], nullptr);
    }
    // pthread_join(p, nullptr);
    // pthread_join(c, nullptr);

    delete rq;
    return 0;
}