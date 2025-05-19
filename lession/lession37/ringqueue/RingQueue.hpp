#pragma once

#include <iostream>
#include <vector>
#include <semaphore.h>
#include <cassert>
#include <pthread.h>

static const int gcap = 100 ;
template <typename T>
class RingQueue
{
//PV操作
private:
    void P(sem_t *s)
    {
        int n = sem_wait(s);
        assert(n == 0);
        (void)n;
    }
    void V(sem_t *s)
    {
        int n = sem_post(s);
        assert(n == 0);
        (void)n;
    }

public:
    RingQueue(const int &cap = gcap) : _queue(cap), _cap(cap)
    {
        int n = sem_init(&_spaceSem, 0, _cap);
        assert(n == 0);
        n = sem_init(&_dataSem, 0, 0);
        assert(n == 0);
        producerStep = 0;
        consumerStep = 0;
        
        pthread_mutex_init(&_pmutex, NULL);
        pthread_mutex_init(&_cmutex, NULL);
    }
    void push(const T &in)
    {
        // pthread_mutex_lock(&_pmutex);
        P(&_spaceSem);//申请信号量
        pthread_mutex_lock(&_pmutex);
        _queue[producerStep++] = in;
        producerStep %= _cap;
        pthread_mutex_unlock(&_pmutex);
        V(&_dataSem);
    }
    void pop(T *out)
    {
        // pthread_mutex_lock(&_cmutex);
        P(&_dataSem);
        pthread_mutex_lock(&_cmutex);

        *out = _queue[consumerStep++]; 
        consumerStep %= _cap;
        pthread_mutex_unlock(&_cmutex);
        V(&_spaceSem);

    }
    ~RingQueue()
    {
        sem_destroy(&_spaceSem);
        sem_destroy(&_dataSem);
        pthread_mutex_destroy(&_pmutex);
        pthread_mutex_destroy(&_cmutex);
    }

private:
    std::vector<T> _queue;
    int _cap;
    sem_t _spaceSem;
    sem_t _dataSem;

    int producerStep;
    int consumerStep;

    pthread_mutex_t _pmutex;
    pthread_mutex_t _cmutex;
};