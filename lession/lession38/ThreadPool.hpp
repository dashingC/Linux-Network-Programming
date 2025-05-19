#pragma once
#include "Thread.hpp"
#include "lockGuard.hpp"
#include <vector>
#include <queue>
#include <pthread.h>
#include <unistd.h>
#include <mutex>

using std::cout;
using std::endl;

using namespace ThreadNs;
const int gnum = 10;

template <typename T>
class ThreadPool;

template <typename T>
class ThreadData
{
public:
    ThreadPool<T> *threadpool;
    std::string name;

public:
    ThreadData(ThreadPool<T> *tp, const std::string &n) : threadpool(tp), name(n)
    {
    }
};

template <typename T>
class ThreadPool
{
private:
    static void *handlerTask(void *args)
    {
        ThreadData<T> *td = static_cast<ThreadData<T> *>(args);
        while (true)
        {
            T task;
            // td->threadpool->lockQueue(); // 线程锁定
            {
                LockGuard lockguard(td->threadpool->getMutex()); // RAII锁定，自动解锁
                while (td->threadpool->isQueueEmpty())
                {
                    td->threadpool->threadWait(); // 线程等待
                }

                task = td->threadpool->pop(); // 取出任务
            }

            // td->threadpool->unlockQueue();

            std::cout << td->name << "获取任务：" << task.toTaskString() << "并处理完成，结果是" << task() << std::endl;
        }
        delete td;
        return nullptr;
    }
    ThreadPool(const int &num = gnum) : _num(num)
    {
        pthread_mutex_init(&_mutex, nullptr);
        pthread_cond_init(&_cond, nullptr);
        for (int i = 0; i < _num; i++)
        {
            _threads.push_back(new Thread());
        }
    }
    void operator=(const ThreadPool &) = delete; // 禁止赋值
    ThreadPool(const ThreadPool &) = delete;     // 禁止拷贝构造

public:
    void lockQueue() { pthread_mutex_lock(&_mutex); }
    void unlockQueue() { pthread_mutex_unlock(&_mutex); }
    bool isQueueEmpty() { return _taskQueue.empty(); }
    void threadWait() { pthread_cond_wait(&_cond, &_mutex); }
    pthread_mutex_t *getMutex() { return &_mutex; }

    T pop() // 从任务队列中取出一个任务
    {
        T task = _taskQueue.front();
        _taskQueue.pop();
        return task;
    }

public:
    void run()
    {
        for (auto &thread : _threads)
        {
            ThreadData<T> *td = new ThreadData<T>(this, thread->threadname());
            thread->start(handlerTask, td);
            cout << thread->threadname() << " is started........." << endl;
        }
    }
    void push(const T &task) // 向任务队列添加任务
    {
        // pthread_mutex_lock(&_mutex);
        LockGuard lockguard(&_mutex); // RAII锁定，自动解锁
        _taskQueue.push(task);
        pthread_cond_signal(&_cond); // 唤醒一个等待的线程
        // pthread_mutex_unlock(&_mutex);
    }

    ~ThreadPool()
    {
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_cond);
        for (auto &thread : _threads)
        {
            delete thread;
        }
    }

    static ThreadPool<T> *getInstance()
    {
        if (nullptr == tp)
        {
            _singlelock.lock();
            if (tp == nullptr)
            {
                tp = new ThreadPool<T>();
            }
            _singlelock.unlock();
        }    
            return tp;        
    }

private:
    int _num;
    std::vector<Thread *> _threads;
    std::queue<T> _taskQueue;
    pthread_mutex_t _mutex;
    pthread_cond_t _cond;
    static std::mutex _singlelock;

    static ThreadPool<T> *tp; // 构建静态指针
};

template <typename T>
ThreadPool<T> *ThreadPool<T>::tp = nullptr; // 初始化静态指针

template <typename T>
std::mutex ThreadPool<T>::_singlelock;