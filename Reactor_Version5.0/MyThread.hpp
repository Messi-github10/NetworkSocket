#ifndef MYTHREAD_HPP
#define MYTHREAD_HPP

#include "Noncopyable.hpp"
#include <pthread.h>
#include <functional>
using namespace std;

// MyThread只是管理线程的对象，并非线程本身
using ThreadCallback = function<void()>;    // 定义回调函数指针类型
class MyThread : public Noncopyable{
public:
    // ThreadCallback就是线程的入口函数
    MyThread(ThreadCallback &&threadCallback)
    :_pthread_id(0)
    ,_isRunning(false)
    ,_threadCallback(threadCallback)
    {}

    ~MyThread() = default;

    void start();   // 开始线程（实际在此时才创建线程）
    void join();    // 等待目标线程执行完成


private:
    static void *start_routine(void *);

private:
    pthread_t _pthread_id;  // 当前线程的id
    bool _isRunning;        // 是否在运行
    ThreadCallback _threadCallback; // 回调函数对象
};

#endif