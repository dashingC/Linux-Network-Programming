#pragma once

#include <iostream>
#include <functional>



class Task
{
    using func_t = std::function<int(int, int,char)>;
public:
    Task()
    {}
    Task(int x, int y, char op,func_t callback): _x(x), _y(y),_op(op), _callback(callback)
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
public:
    int _x;
    int _y;
    char _op;
    func_t _callback;

};