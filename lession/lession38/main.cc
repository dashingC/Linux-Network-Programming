#include "ThreadPool.hpp"
#include "Thread.hpp"
#include "Task.hpp"
#include <iostream>
#include <unistd.h>
#include <memory>

int main()
{
    // std::unique_ptr<ThreadPool<Task>> threadpool(new ThreadPool<Task>());

    // threadpool->run();
    ThreadPool<Task>::getInstance()->run();

    int value1;
    int value2;
    char op;
    while (true)
    {

        std::cout << "请输入 数据1 : " ;
        std::cin >> value1;

        std::cout << "请输入操作符 : ";
        std::cin >> op;
        if (op != '+' && op != '-' && op != '*' && op != '/')
        {
            std::cout << "操作符错误，请重新输入" << std::endl;
            continue;
        }

        std::cout << "请输入 数据2 : ";
        std::cin >> value2;

        Task t(value1, value2, op, mymath);

        ThreadPool<Task>::getInstance()->push(t);

        sleep(1);
    }
    return 0;
}