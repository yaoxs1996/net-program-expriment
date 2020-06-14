#ifndef EPOLLEXCEPTION_H
#define EPOLLEXCEPTION_H

#include <string>
#include <cstring>

class EpollException
{
public:
    typedef std::string string;
    EpollException(const string &_msg = string())
        : msg(_msg) {}
    string what() const
    {
        if (errno == 0)
            return msg;
        //如果errno!=0, 则会加上错误描述
        return msg + ": " + strerror(errno);
    }

private:
    string msg;
};

#endif