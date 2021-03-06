#ifndef TASK_H
#define TASK_H

#include <pthread.h>
#include <unistd.h>
#include <deque>
#include <iostream>
#include <vector>
#include <errno.h>
#include <string.h>

using namespace std;

class Task
{
public:
    Task(void (*fn_ptr)(void*), void* arg); // pass a free function pointer
    ~Task();
    void operator()();
    void run();
private:
    void (*m_fn_ptr)(void*);
    void* m_arg;
};

#endif