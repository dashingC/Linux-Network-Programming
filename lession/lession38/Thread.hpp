#pragma once

#include <iostream>
#include <pthread.h>
#include <functional>
#include <cassert>
#include <string>
#include <cstring>

namespace ThreadNs
{
    typedef std::function<void *(void *)> func_t;
    const int num = 1024;

    class Thread
    {
    private:
        // 在类内创建线程，想让线程执行对应的方法，需要讲方法设置为static  静态成员函数属于类本身，而不是类的任何特定对象。
        static void *start_routine(void *args)
        {
            Thread *_this = static_cast<Thread *>(args); // this指针是隐式传递的，static_cast<Thread *>(args)将args转换为Thread类型的指针
            return _this->callback();
        }

        void *callback()
        {
            return func_(args_);
        }

    public:
        Thread()
        {
            char namebuffer[num];
            snprintf(namebuffer, sizeof namebuffer, "thread_%d", thread_num++); // 执行后，buffer 会包含一个以 空字符 \0 结尾的C风格字符串（前提是 buffer 大小足够）
            name_ = namebuffer;
        }

        void start(func_t func, void *args = nullptr)
        {
            func_ = func;
            args_ = args;
            int n = pthread_create(&tid_, nullptr, start_routine, this);
            assert(n == 0); // 编译debug的方式进行发布的时候存在，release方式发布，assert就不存在了，n就是一个定义了，但是没有被使用的变量
            (void)n;        // 在有些编译器下会有warning
        }
        void join()
        {
            int n = pthread_join(tid_, nullptr);
            assert(n == 0);
            (void)n;
        }
        std::string threadname()
        {
            return name_;
        }

        ~Thread()
        {
        }

    private:
        std::string name_;
        func_t func_;
        void *args_;
        pthread_t tid_;
        static int thread_num;
    };
    int Thread::thread_num = 1;

} // 命名空间结束