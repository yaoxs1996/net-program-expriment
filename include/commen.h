#ifndef COMMEN_H
#define COMMEN_H

#include "epoll.h"
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/resource.h>

#include <poll.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <semaphore.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <future>
#include <memory>
#include <iostream>
#include <algorithm>
using namespace std;

#include "Socket.h"

void sigHandlerForSigChild(int signo)
{
    while (waitpid(-1, NULL, WNOHANG) > 0)
        ;
}
void sigHandlerForSigPipe(int signo)
{
    cerr << "recv a signal " << signo << ": " << strsignal(signo) << endl;
}

/**返回值说明:
    == count: 说明正确返回, 已经真正读取了count个字节
    == -1   : 读取出错返回
    <  count: 读取到了末尾
**/
inline ssize_t readn(int fd, void *buf, size_t count)
{
    size_t nLeft = count;
    ssize_t nRead = 0;
    char *pBuf = (char *)buf;
    while (nLeft > 0)
    {
        if ((nRead = read(fd, pBuf, nLeft)) < 0)
        {
            //如果读取操作是被信号打断了, 则说明还可以继续读
            if (errno == EINTR)
                continue;
            //否则就是其他错误
            else
                return -1;
        }
        //读取到末尾
        else if (nRead == 0)
            return count-nLeft;

        //正常读取
        nLeft -= nRead;
        pBuf += nRead;
    }
    return count;
}
/**返回值说明:
    == count: 说明正确返回, 已经真正写入了count个字节
    == -1   : 写入出错返回
**/
inline ssize_t writen(int fd, const void *buf, size_t count)
{
    size_t nLeft = count;
    ssize_t nWritten = 0;
    char *pBuf = (char *)buf;
    while (nLeft > 0)
    {
        if ((nWritten = write(fd, pBuf, nLeft)) < 0)
        {
            //如果写入操作是被信号打断了, 则说明还可以继续写入
            if (errno == EINTR)
                continue;
            //否则就是其他错误
            else
                return -1;
        }
        //如果 ==0则说明是什么也没写入, 可以继续写
        else if (nWritten == 0)
            continue;

        //正常写入
        nLeft -= nWritten;
        pBuf += nWritten;
    }
    return count;
}

inline void err_exit(const std::string &msg)
{
    perror(msg.c_str());
    exit(EXIT_FAILURE);
}
inline void err_quit(const std::string &msg)
{
    std::cerr << msg << std::endl;
    exit(EXIT_FAILURE);
}
ssize_t recv_peek(int sockfd, void *buf, size_t len)
{
    while (true)
    {
        int ret = recv(sockfd, buf, len, MSG_PEEK);
        //如果recv是由于被信号打断, 则需要继续(continue)查看
        if (ret == -1 && errno == EINTR)
            continue;
        return ret;
    }
}
/** 返回值说明:
    == 0:   对端关闭
    == -1:  读取出错
    其他:    一行的字节数(包含'\n')
**/
ssize_t readline(int sockfd, void *buf, size_t maxline)
{
    int ret;
    int nRead = 0;
    int returnCount = 0;
    char *pBuf = (char *)buf;
    int nLeft = maxline;
    while (true)
    {
        ret = recv_peek(sockfd, pBuf, nLeft);
        //如果查看失败或者对端关闭, 则直接返回
        if (ret <= 0)
            return ret;
        nRead = ret;
        for (int i = 0; i < nRead; ++i)
            //在当前查看的这段缓冲区中含有'\n', 则说明已经可以读取一行了
            if (pBuf[i] == '\n')
            {
                //则将缓冲区内容读出
                //注意是i+1: 将'\n'也读出
                ret = readn(sockfd, pBuf, i+1);
                if (ret != i+1)
                    exit(EXIT_FAILURE);
                return ret + returnCount;
            }

        // 如果在查看的这段消息中没有发现'\n', 则说明还不满足一条消息,
        // 在将这段消息从缓冲中读出之后, 还需要继续查看
        ret = readn(sockfd, pBuf, nRead);;
        if (ret != nRead)
            exit(EXIT_FAILURE);
        pBuf += nRead;
        nLeft -= nRead;
        returnCount += nRead;
    }
    //如果程序能够走到这里, 则说明是出错了
    return -1;
}

/**
 *read_timeout - 读超时检测函数, 不包含读操作
 *@fd: 文件描述符
 *@waitSec: 等待超时秒数, 0表示不检测超时
 *成功(未超时)返回0, 失败返回-1, 超时返回-1 并且 errno = ETIMEDOUT
**/
int read_timeout(int fd, long waitSec)
{
    int returnValue = 0;
    if (waitSec > 0)
    {
        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(fd,&readSet);    //添加

        struct timeval waitTime;
        waitTime.tv_sec = waitSec;
        waitTime.tv_usec = 0;       //将微秒设置为0(不进行设置),如果设置了,时间会更加精确
        do
        {
            returnValue = select(fd+1,&readSet,NULL,NULL,&waitTime);
        }
        while(returnValue < 0 && errno == EINTR);   //等待被(信号)打断的情况, 重启select

        if (returnValue == 0)   //在waitTime时间段中一个事件也没到达
        {
            returnValue = -1;   //返回-1
            errno = ETIMEDOUT;
        }
        else if (returnValue == 1)  //在waitTime时间段中有事件产生
            returnValue = 0;    //返回0,表示成功
        // 如果(returnValue == -1) 并且 (errno != EINTR), 则直接返回-1(returnValue)
    }

    return returnValue;
}


/**
 *write_timeout - 写超时检测函数, 不包含写操作
 *@fd: 文件描述符
 *@waitSec: 等待超时秒数, 0表示不检测超时
 *成功(未超时)返回0, 失败返回-1, 超时返回-1 并且 errno = ETIMEDOUT
**/
int write_timeout(int fd, long waitSec)
{
    int returnValue = 0;
    if (waitSec > 0)
    {
        fd_set writeSet;
        FD_ZERO(&writeSet);      //清零
        FD_SET(fd,&writeSet);    //添加

        struct timeval waitTime;
        waitTime.tv_sec = waitSec;
        waitTime.tv_usec = 0;
        do
        {
            returnValue = select(fd+1,NULL,&writeSet,NULL,&waitTime);
        }
        while(returnValue < 0 && errno == EINTR);   //等待被(信号)打断的情况

        if (returnValue == 0)   //在waitTime时间段中一个事件也没到达
        {
            returnValue = -1;   //返回-1
            errno = ETIMEDOUT;
        }
        else if (returnValue == 1)  //在waitTime时间段中有事件产生
            returnValue = 0;    //返回0,表示成功
    }

    return returnValue;
}

/**
 *accept_timeout - 带超时的accept
 *@fd: 文件描述符
 *@addr: 输出参数, 返回对方地址
 *@waitSec: 等待超时秒数, 0表示不使用超时检测, 使用正常模式的accept
 *成功(未超时)返回0, 失败返回-1, 超时返回-1 并且 errno = ETIMEDOUT
**/
int accept_timeout(int fd, struct sockaddr_in *addr, long waitSec)
{
    int returnValue = 0;
    if (waitSec > 0)
    {
        fd_set acceptSet;
        FD_ZERO(&acceptSet);
        FD_SET(fd,&acceptSet);    //添加

        struct timeval waitTime;
        waitTime.tv_sec = waitSec;
        waitTime.tv_usec = 0;
        do
        {
            returnValue = select(fd+1,&acceptSet,NULL,NULL,&waitTime);
        }
        while(returnValue < 0 && errno == EINTR);

        if (returnValue == 0)  //在waitTime时间段中没有事件产生
        {
            errno = ETIMEDOUT;
            return -1;
        }
        else if (returnValue == -1) // error
            return -1;
    }

    /**select正确返回:
        表示有select所等待的事件发生:对等方完成了三次握手,
        客户端有新的链接建立,此时再调用accept就不会阻塞了
    */
    socklen_t socklen = sizeof(struct sockaddr_in);
    if (addr != NULL)
        returnValue = accept(fd,(struct sockaddr *)addr,&socklen);
    else
        returnValue = accept(fd,NULL,NULL);

    return returnValue;
}

/**设置文件描述符fd为非阻塞/阻塞模式**/
bool setUnBlock(int fd, bool unBlock)
{
    int flags = fcntl(fd,F_GETFL);
    if (flags == -1)
        return false;

    if (unBlock)
        flags |= O_NONBLOCK;
    else
        flags &= ~O_NONBLOCK;

    if (fcntl(fd,F_SETFL,flags) == -1)
        return false;
    return true;
}
/**
 *connect_timeout - connect
 *@fd: 文件描述符
 *@addr: 要连接的对方地址
 *@waitSec: 等待超时秒数, 0表示使用正常模式的accept
 *成功(未超时)返回0, 失败返回-1, 超时返回-1 并且 errno = ETIMEDOUT
**/
int connect_timeout(int fd, struct sockaddr_in *addr, long waitSec)
{
    if (waitSec > 0)    //设置为非阻塞模式
        setUnBlock(fd, true);

    socklen_t addrLen = sizeof(struct sockaddr_in);
    //首先尝试着进行链接
    int returnValue = connect(fd,(struct sockaddr *)addr,addrLen);
    //如果首次尝试失败(并且errno == EINPROGRESS表示连接正在处理当中),则需要我们的介入
    if (returnValue < 0 && errno == EINPROGRESS)
    {
        fd_set connectSet;
        FD_ZERO(&connectSet);
        FD_SET(fd,&connectSet);
        struct timeval waitTime;
        waitTime.tv_sec = waitSec;
        waitTime.tv_usec = 0;
        do
        {
            /*一旦建立链接,则套接字可写*/
            returnValue = select(fd+1, NULL, &connectSet, NULL, &waitTime);
        }
        while (returnValue < 0 && errno == EINTR);
        if (returnValue == -1) //error
            return -1;
        else if (returnValue == 0)   //超时
        {
            returnValue = -1;
            errno = ETIMEDOUT;
        }
        else if (returnValue == 1)  //正确返回,有一个套接字可写
        {
            /**由于connectSet只有一个文件描述符, 因此FD_ISSET的测试也就省了**/

            /**注意:套接字可写有两种情况:
                1.连接建立成功
                2.套接字产生错误(但是此时select是正确的, 因此错误信息没有保存在errno中),需要调用getsockopt获取
            */
            int err;
            socklen_t errLen = sizeof(err);
            int sockoptret = getsockopt(fd,SOL_SOCKET,SO_ERROR,&err,&errLen);
            if (sockoptret == -1)
                return -1;

            // 测试err的值
            if (err == 0)   //确实是链接建立成功
                returnValue = 0;
            else    //连接产生了错误
            {
                errno = err;
                returnValue = -1;
            }
        }
    }
    if (waitSec > 0)
        setUnBlock(fd, false);
    return returnValue;
}
#endif