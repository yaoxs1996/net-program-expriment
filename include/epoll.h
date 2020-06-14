#ifndef EPOLL_H
#define EPOLL_H

#include <unistd.h>
#include <sys/epoll.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <strings.h>
#include <vector>
#include "epollException.h"

class Epoll
{
public:
    Epoll(int flags = EPOLL_CLOEXEC, int noFile = 1024);
    ~Epoll();

    void addfd(int fd, uint32_t events = EPOLLIN, bool ETorNot = false);
    void modfd(int fd, uint32_t events = EPOLLIN, bool ETorNot = false);
    void delfd(int fd);
    int wait(int timeout = -1);
    int getEventOccurfd(int eventIndex) const;
    uint32_t getEvents(int eventIndex) const;

public:
    bool isValid()
    {
        if (m_epollfd == -1)
            return false;
        return true;
    }
    void close()
    {
        if (isValid())
        {
            :: close(m_epollfd);
            m_epollfd = -1;
        }
    }

private:
    std::vector<struct epoll_event> events;
    int m_epollfd;
    int fdNumber;
    int nReady;
private:
    struct epoll_event event;
};

#endif