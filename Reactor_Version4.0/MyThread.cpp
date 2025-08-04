#include "MyThread.hpp"
#include <stdio.h>
#include <string.h>

void MyThread::start(){
    if(!_isRunning){
        int ret = pthread_create(&_pthread_id, NULL, start_routine, this);
        if(ret != 0){
            fprintf(stderr, "%s", strerror(ret));
            return;
        }
        _isRunning = true;
    }
}

void MyThread::join(){
    if(_isRunning){
        pthread_join(_pthread_id, NULL);
        _isRunning = false;
    }
}

void *MyThread::start_routine(void *arg){
    MyThread *mythread = static_cast<MyThread *>(arg);
    if(mythread != nullptr){
        mythread->_threadCallback();
    }
    return nullptr;
}