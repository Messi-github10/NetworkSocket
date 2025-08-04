#ifndef MYTHREAD_HPP
#define MYTHREAD_HPP

#include "Noncopyable.hpp"
#include <pthread.h>
#include <functional>
using namespace std;

using ThreadCallback = function<void()>;    // 定义回调函数指针类型
class MyThread : public Noncopyable{
public:
    // 相当于多态中的基类指针/引用指向派生类对象（形成产生多态的条件）
    // ThreadCallback就是线程的入口函数
    MyThread(ThreadCallback &&threadCallback)
    :_pthread_id(0)
    ,_isRunning(false)
    ,_threadCallback(threadCallback)
    {}

    ~MyThread() = default;

    void start();
    void join();


private:
    static void *start_routine(void *);

private:
    pthread_t _pthread_id;
    bool _isRunning;
    ThreadCallback _threadCallback; // 回调函数对象
};

#endif