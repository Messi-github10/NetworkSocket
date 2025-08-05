#ifndef MUTEXLOCK_HPP
#define MUTEXLOCK_HPP

#include "Noncopyable.hpp"
#include <pthread.h>

// MutexLock类封装了互斥锁，但并非互斥锁本身
class MutexLock : public Noncopyable{
public:
    // RAII原则
    MutexLock(){
        pthread_mutex_init(&_mutex, nullptr);
    }

    ~MutexLock(){
        pthread_mutex_destroy(&_mutex);
    }

    void lock();    // 上锁
    void unlock();  // 解锁
    pthread_mutex_t *getMutexLockPtr(); // 获取互斥锁的指针

private:
    pthread_mutex_t _mutex;
};

#endif