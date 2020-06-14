#ifndef TIMERWHEEL_H
#define TIMERWHEEL_H

#include <functional>
#include <iostream>
#include <map>
#include <vector>
#include <list>
#include <thread>
#include <sys/timerfd.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <time.h>
#include <error.h>
#include <string.h>

struct param
{
    /*
    ** Posix定时器开始值和间隔
    */
    struct itimerspec its;
    int ifd;
};

/* 定时器任务 */
struct TimerTask
{
    int repeat;
    int timeval;
    int id;
    std::function<void()> task;
};

class TimerWheel
{
public:
    TimerWheel(int tick = 5,int slot_num = (1<<3),int layer_num = (1<<4));
    ~TimerWheel();
    void Start();
    void Stop();
    int AddTask(struct TimerTask &task);

private:
    int init();
    void shedule(int);
    void tick();
private:
    int m_tick;     // 滴答的时间粒度
    int m_slot_num;     // 轮子大小
    int m_layer_num;        // 轮子的层数
    int m_cur_slot_num;
    int m_cur_layer_num;
    int m_status;       //状态：0 = stop , 1 = running
    std::vector<std::vector<std::list<struct TimerTask>>> wheel;

    std::thread *m_run_shedule;     // 运行调度
    int m_timer_fd;
};

#endif