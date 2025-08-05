#ifndef CONDITION_VA_HPP
#define CONDITION_VA_HPP

#include "MutexLock.hpp"
#include <pthread.h>

// 条件变量
class Condition_va{
public:
    // RAII原则
    Condition_va(MutexLock& mutex)
    :_mutex(mutex)
    {
        pthread_cond_init(&_cond, nullptr);
    }

    ~Condition_va(){
        pthread_cond_destroy(&_cond);
    }

    void wait();    // 调用 wait 的线程将会被阻塞
    void notify_one();  // 唤醒一个线程
    void notify_all();  // 唤醒全部线程

private:
    MutexLock &_mutex;
    pthread_cond_t _cond;
};

#endif