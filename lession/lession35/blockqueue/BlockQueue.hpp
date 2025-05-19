#pragma once

#include <iostream>
#include <queue>
#include <pthread.h>


static const int gmaxcap = 10;

template <typename T>
class BlockQueue
{
public:
    BlockQueue(const int &maxcap = gmaxcap) : _maxcap(maxcap)
    {
        pthread_mutex_init(&_mutex, nullptr);
        pthread_cond_init(&_pcond, nullptr);
        pthread_cond_init(&_ccond, nullptr);
    }

    void push(const T &in) // 输入型参数，const &
    {
        // 加锁
        pthread_mutex_lock(&_mutex);

        // 1、判断队列是否已满
        while (is_full())
        {
            pthread_cond_wait(&_pcond, &_mutex); // 因为生产条件不满足，无法生产，此时我们的生产者在这里进行阻塞等待
        }
        // 2、走到这里一定是没有满
        _q.push(in);
        // 3、绝对能保证，阻塞队列里面有数据,通知消费者，生产者已经生产了数据
        pthread_cond_signal(&_ccond);

        // 解锁
        pthread_mutex_unlock(&_mutex);
    }
    void pop(T *out) // 输出型参数，T * ,//输入输出型参数 &
    {
        // 加锁
        pthread_mutex_lock(&_mutex);

        // 1、判断队列是否为空
        //细节2：充当条件判断的语法必须是while循环，不能是if语句
        //   这是因为在多线程的情况下，可能会出现虚假唤醒的情况,比如信号唤醒的时候用的广播，导致多个线程被唤醒
        //   如果发生虚假唤醒，线程直接跳过 if，不再检查条件，然后操作空队列 → 出错
        //   使用while即使被“误唤醒”，它还会再次检查“队列是否为空”，不满足则继续等待，就不会出错
        
        while (is_empty())
        {
            //细节1：pthread_cond_wait()函数的第一个参数是条件变量，第二个参数必须是我们正在使用的互斥锁
            //a. pthread_cond_wait()函数 在调用的时候，会以原子操作的方式，先解锁互斥锁，然后进行阻塞等待
            //b. pthread_cond_wait()函数在被唤醒的时候，会自动加锁互斥锁
            pthread_cond_wait(&_ccond, &_mutex); // 因为消费条件不满足，无法消费，此时我们的消费者在这里进行阻塞等待
        }
        // 2、走到这里一定有数据
        *out = _q.front();
        _q.pop();
        // 3、绝对能保证，阻塞队列里面至少有一个空的位置,通知生产者，可以进行生产
        //细节3： pthread_cond_signal可以放在临界区内部也可以放在临界区外部
        pthread_cond_signal(&_pcond);

        // 解锁
        pthread_mutex_unlock(&_mutex);
    }

private:
    bool is_empty()
    {
        return _q.empty();
    }
    bool is_full()
    {
        return _q.size() == _maxcap;
    }

    ~BlockQueue()
    {
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_pcond);
        pthread_cond_destroy(&_ccond);
    }

private:
    std::queue<T> _q;
    int _maxcap;
    pthread_mutex_t _mutex;
    pthread_cond_t _pcond;
    pthread_cond_t _ccond;
};
