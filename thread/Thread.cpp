#include "Thread.h"
#include <iostream>

Thread::Thread()
{
    context = 0;
}

Thread::~Thread()
{
}

int Thread::run(void *pContext)
{
    context = pContext;

    int r = pthread_create(&threadID, 0, startFunctionOfThread, this);
    if(r != 0)
    {
        printf("In Thread::run(), pthread_create error.\n");
        return -1;
    }
    return 0;
}

int Thread::waitForDeath()
{
    int r = pthread_join(threadID, 0);
    if(r != 0)
    {
        printf("In Thread::waitForDeath(), pthread_join error.\n");
        return -1;
    }
    return 0;
}

void* Thread::startFunctionOfThread(void *pThis)
{
    Thread *threadThis = (Thread *)pThis;
    int s = threadThis->runThreadFunction();
    return (void *)s;
}