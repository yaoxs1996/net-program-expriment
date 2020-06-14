#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>

class Thread
{
public:
    Thread();
    virtual ~Thread();

    int run(void *pContext = 0);
    int waitForDeath();

private:
    static void* startFunctionOfThread(void *context);

protected:
    virtual int runThreadFunction() = 0;
    void *context;
    pthread_t threadID;
};

#endif