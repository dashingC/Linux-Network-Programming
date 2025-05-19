#pragma once

#include <iostream>
#include <pthread.h>
#include <functional>
#include <cassert>
#include <string>
#include <cstring>

class Thread;

// 上下文
class Context
{
public:
    Thread *this_;
    void *args_;

public:
    Context() : this_(nullptr), args_(nullptr)
    {
    }
    ~Context()
    {
    }
};

class Thread
{
public:
    typedef std::function<void *(void *)> func_t;
    const int num = 1024;

private:
    std::string name_;
    func_t func_;
    void *args_;

    pthread_t tid_;

public:
    Thread(func_t func, void *args, int number);
    ~Thread();
    static void *start_routine(void *args);
    void join();
    void *run(void *args);
};

Thread::Thread(func_t func, void *args, int number) : func_(func), args_(args)
{
    char buffer[num];
    snprintf(buffer, sizeof buffer, "thread_%d", number); // 执行后，buffer 会包含一个以空字符 \0 结尾的C风格字符串（前提是 buffer 大小足够）
    name_ = buffer;
    // name_ 是一个 std::string 类型的对象。
    // std::string 类有一个赋值操作符（可以接受一个 const char* 类型的参数（C风格字符串）。
    // 执行 name_ = buffer; 时，std::string 对象需要知道C风格字符串 buffer 的长度，以便分配足够的内存并复制内容。
    //  标准库中确定C风格字符串长度的函数是 strlen()。strlen() 函数是在 <cstring>头文件中声明的，所以需要<cstring>头文件

    // 异常 == if:意料之外用异常或者if判断
    // assert：意料之中用assert
    Context *ctx = new Context();
    ctx->this_ = this;
    ctx->args_ = args_;

    int n = pthread_create(&tid_, nullptr, start_routine, ctx);
    assert(n == 0); // 编译debug的方式进行发布的时候存在，release方式发布，assert就不存在了，n就是一个定义了，但是没有被使用的变量
    // 在有些编译器下会有warning
    (void)n;
}

Thread::~Thread()
{
    // do nothing
}

// 在类内创建线程，想让线程执行对应的方法，需要讲方法设置为static
// 静态成员函数属于类本身，而不是类的任何特定对象。
void *Thread::start_routine(void *args) // 类内成员，有缺省参数！this指针
{
    // 因此，静态成员函数没有 this 指针。它们不能直接访问类的非静态成员变量或调用非静态成员函数
    Context *ctx = static_cast<Context *>(args);
    void *ret = ctx->this_->run(ctx->args_);
    delete ctx;
    return ret;
}

void Thread::join()
{
    int n = pthread_join(tid_, nullptr);
    assert(n == 0);
    (void)n;
}
void *Thread::run(void *args)
{
    return func_(args);
}
