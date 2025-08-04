#include "Threadpool.hpp"
#include <functional>
#include <unistd.h>

Threadpool::Threadpool(int threadNum, int queSize)
:_threads()
,_threadNum(threadNum)
,_taskque(queSize)
,_queSize(queSize)
,_isExit(false)
{
    // 预分配线程池空间
    _threads.reserve(_threadNum);
}

void Threadpool::startThreadPool(){
    // 创建线程对象
    for (int i = 0; i < _threadNum; i++){
        unique_ptr<MyThread> pTh(new MyThread(bind(&Threadpool::doTask, this)));
        _threads.push_back(std::move(pTh));
    }

    // 让每个线程运行起来
    for(auto & thread : _threads){
        thread->start();
    }
}

void Threadpool::stopThreadPool(){
    while(!_taskque.empty()){
        sleep(1);
    }

    _isExit = true;

    _taskque.wakeUpAllThread();

    // 等待每一个子线程完成
    for(auto  & thread : _threads){
        thread->join();
    }
}

void Threadpool::addTask(Task &&task){
    if(task){
        _taskque.push(std::move(task));
    }
}

void Threadpool::doTask(){
    while(!_isExit){
        Task task = _taskque.pop();
        if(task){
            task();
        }
    }
}