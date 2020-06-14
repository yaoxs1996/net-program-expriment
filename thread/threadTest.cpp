#include <iostream>
#include <unistd.h>
#include <stack>

#include "Thread.h"
#include "Cond.h"
#include "Mutex.h"

using namespace std;

static stack<int> resourse;
static Mutex mutex;
static Cond cond;

/* 生产者进程类 */
class ProducerThread : public Thread
{
public:
    virtual int runThreadFunction()
    {
        data = rand() % 1234;
        while(1)
        {
            sleep(4);
            mutex.lock();
            resourse.push(data);
            cout << "Producer data: " << data << endl;
            mutex.unlock();
            cond.signal();
        }
    }
private:
    int data;
};

/* 消费者进程类 */
class ConsumerThread : public Thread
{
public:
    virtual int runThreadFunction()
    {
        int data;
        while(1)
        {
            mutex.lock();
            while(resourse.empty())
            {
                cout << "Producer is not ready" << endl << endl;
                cond.wait(mutex.get_mutex_ptr());
                break;
            }
            
            //cout << "Producer is ready" << endl;
            data = resourse.top();
            resourse.pop();
            cout << "Consumer data = " << data << endl;
            
            mutex.unlock();
        }
        sleep(1);
    }
private:
    int data;
};

int main()
{
    Thread *t1 = new ProducerThread;
    Thread *t2 = new ConsumerThread;

    t1->run();
    t2->run();

    t1->waitForDeath();
    t2->waitForDeath();

    delete t1;
    delete t2;

    return 0;
}