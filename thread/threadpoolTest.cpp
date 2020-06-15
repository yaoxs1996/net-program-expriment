#include "ThreadPool.h"

#include <iostream>
#include <stack>

using namespace std;

const int MAX_TASKS = 4;

static stack<int> resourse;     // 资源
static Mutex mutex;
static Cond cond;

/*void hello(void* arg)
{
    int* x = (int*) arg;
    cout << "Hello " << *x << endl;
}*/

void producer(void *arg)
{
    int data = rand() % 1234;
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

void consumer(void *arg)
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

int main(int argc, char* argv[])
{
    ThreadPool tp(2);
    int ret = tp.initialize_threadpool();
    if (ret == -1) {
        cerr << "初始化线程池失败！" << endl;
        return 0;
    }

    /*for (int i = 0; i < MAX_TASKS; i++) {
        int* x = new int();
        *x = i+1;
        Task* t = new Task(&hello, (void*) x);
        tp.add_task(t);
    }*/

    Task *t1 = new Task(&producer, (void *)0);
    Task *t2 = new Task(&consumer, (void *)0);
    tp.add_task(t1);
    tp.add_task(t2);

    sleep(2);

    tp.destroy_threadpool();
    //cout << "Exiting app..." << endl;

    return 0;
}