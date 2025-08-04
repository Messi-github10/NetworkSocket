#include "MutexLock.hpp"

void MutexLock::lock(){
    pthread_mutex_lock(&_mutex);
}

void MutexLock::unlock(){
    pthread_mutex_unlock(&_mutex);
}

pthread_mutex_t *MutexLock::getMutexLockPtr(){
    return &_mutex;
}
