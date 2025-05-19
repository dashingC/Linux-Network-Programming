#pragma once

#include <iostream>
#include <pthread.h>

class Mutex
{
public:
    Mutex(pthread_mutex_t *mutex_p=nullptr) : lock_p_(mutex_p)
    {}

    void lock()
    {
        if (lock_p_)
        {
            pthread_mutex_lock(lock_p_);
        }
    }
    void unlock()
    {
        if (lock_p_)
        {
            pthread_mutex_unlock(lock_p_);
        }
    }
    ~Mutex()
    {}

private:
    pthread_mutex_t *lock_p_;
};

class LockGuard
{
public:
    LockGuard(pthread_mutex_t * mutex):mutex_(mutex)
    {
        mutex_.lock();//在构造函数中加锁
    }
    ~LockGuard()
    {
        mutex_.unlock();//在析构函数中解锁
    }
private:
    Mutex mutex_;
};