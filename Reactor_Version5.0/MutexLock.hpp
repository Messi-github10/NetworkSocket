#ifndef MUTEXLOCK_HPP
#define MUTEXLOCK_HPP

#include "Noncopyable.hpp"
#include <pthread.h>

class MutexLock : public Noncopyable{
public:
    MutexLock(){
        pthread_mutex_init(&_mutex, nullptr);
    }

    ~MutexLock(){
        pthread_mutex_destroy(&_mutex);
    }

    void lock();
    void unlock();
    pthread_mutex_t *getMutexLockPtr();

private:
    pthread_mutex_t _mutex;
};

#endif