#include "MyThread.hpp"
#include <stdio.h>
#include <string.h>

void MyThread::start(){
    if(!_isRunning){
        // 创建线程并指定入口函数
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
        // 等待目标线程 _pthread_id 执行结束
        pthread_join(_pthread_id, NULL);
        _isRunning = false;
    }
}

void *MyThread::start_routine(void *arg){
    MyThread *mythread = static_cast<MyThread *>(arg);  // arg 的类型是 void *，需要强制类型转换成目标类型
    if(mythread != nullptr){
        mythread->_threadCallback();    // 类似面向对象中的基类指针指向派生类对象
    }
    return nullptr;
}