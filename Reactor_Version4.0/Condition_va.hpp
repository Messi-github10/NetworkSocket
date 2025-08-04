#ifndef CONDITION_VA_HPP
#define CONDITION_VA_HPP

#include "MutexLock.hpp"
#include <pthread.h>

class Condition_va{
public:
    Condition_va(MutexLock& mutex)
    :_mutex(mutex)
    {
        pthread_cond_init(&_cond, nullptr);
    }

    ~Condition_va(){
        pthread_cond_destroy(&_cond);
    }

    void wait();
    void notify_one();
    void notify_all();

private:
    MutexLock &_mutex;
    pthread_cond_t _cond;
};

#endif