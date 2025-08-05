#ifndef SMARTMUTEXLOCK_HPP
#define SMARTMUTEXLOCK_HPP

#include "MutexLock.hpp"
#include <pthread.h>

// 类似智能指针的互斥锁
class SmartMutexLock{
public:

    // RAII
    SmartMutexLock(MutexLock &mutex)
    :_mutex(mutex)
    {
        pthread_mutex_lock(_mutex.getMutexLockPtr());
    }

    ~SmartMutexLock(){
        pthread_mutex_unlock(_mutex.getMutexLockPtr());
    }

private:
    MutexLock &_mutex;
};

#endif