#ifndef COND_H
#define COND_H

#include <pthread.h>
#include <unistd.h>
#include <deque>
#include <iostream>
#include <vector>

using namespace std;

class Cond {
public:

    Cond();
    ~Cond();
    void wait(pthread_mutex_t* mutex);
    void signal();
    void broadcast();

private:
    pthread_cond_t m_cond_var;
};

#endif