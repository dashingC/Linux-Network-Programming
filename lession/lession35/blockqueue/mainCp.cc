#include "BlockQueue.hpp"
#include "Task.hpp"
#include <unistd.h>
#include <ctime>
#include <pthread.h>
#include <iostream>
const std::string oper = "+-*/";

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;


int mymath(int x, int y, char op)
{
    switch (op)
    {
    case '+':
        return x + y;
        break;
    case '-':
        return x - y;
        break;
    case '*':
        return x * y;
        break;
    case '/':
        if (y == 0)
        {
            std::cout << "除数不能为0" << std::endl;
            return 0;
        }
        return x / y;
        break;

    default:
        std::cout << "不支持的操作符" << std::endl;
        return 0;
    }
}
void *producer(void *bq_)
{
    BlockQueue<Task> *bq = static_cast<BlockQueue<Task> *>(bq_);
    while (true)
    {
        // 生产活动
        int x = rand() % 10 + 1;
        int y = rand() % 5 + 1;
        char operCode = oper[rand() % oper.size()];
        Task t(x, y,operCode, mymath);
        bq->push(t); // 生产者生产数据
        pthread_mutex_lock(&lock);
        std::cout << "生产任务:" << t.toTaskString() << std::endl;
        pthread_mutex_unlock(&lock);
        // sleep(1);
    }
}
void *consumer(void *bq_)
{
    BlockQueue<Task> *bq = static_cast<BlockQueue<Task> *>(bq_);
    while (true)
    {
        // 消费活动
        Task t;
        bq->pop(&t); // 消费者消费数据
        pthread_mutex_lock(&lock);
        std::cout << "消费任务......................" << t() << std::endl;
        pthread_mutex_unlock(&lock);
        sleep(1);
    }
}

int main()
{
    pthread_t p, c;

    BlockQueue<Task> *bq = new BlockQueue<Task>();

    pthread_create(&p, nullptr, producer, bq);
    pthread_create(&c, nullptr, consumer, bq);

    pthread_join(p, nullptr);
    pthread_join(c, nullptr);

    return 0;
}