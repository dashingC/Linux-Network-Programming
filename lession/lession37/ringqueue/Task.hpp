#pragma once

#include <iostream>
#include <functional>
#include <string>
#include <cstdio>



class Task
{
    using func_t = std::function<int(int, int,char)>;
public:
    Task()
    {}
    Task(int x, int y, char op,func_t callback)
    : _x(x), _y(y),_op(op), _callback(callback)
    {}

    std::string operator()()
    {

        int result = _callback(_x, _y, _op);
        char buffer[1024];
        snprintf(buffer, sizeof(buffer), " %d %c %d = %d", _x, _op,_y , result);
        return buffer;
    }
    std::string toTaskString()
    {
        char buffer[1024];
        snprintf(buffer, sizeof(buffer), "%d %c %d = ?", _x, _op,_y);
        return buffer;
    }
private:
    int _x;
    int _y;
    char _op;
    func_t _callback;

};

const std::string oper = "+-*/";

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