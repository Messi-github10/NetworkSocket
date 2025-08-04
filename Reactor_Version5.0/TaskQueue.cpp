#include "TaskQueue.hpp"
#include "SmartMutexLock.hpp"

bool TaskQueue::full() const{
    return (_que.size() == _queSize);
}

bool TaskQueue::empty() const{
    return _que.empty();
}

void TaskQueue::push(ElemType elem){
    SmartMutexLock smartMutex(_mutex);
    while (full())
    {
        _notFull.wait();
    }
    _que.push(elem);
    _notEmpty.notify_one();
}

ElemType TaskQueue::pop(){
    SmartMutexLock SmartMutexLock(_mutex);
    ElemType result = nullptr;
    while (empty() && !_is_stop)
    {
        _notEmpty.wait();
    }

    if(!_is_stop){
        result =  _que.front();
        _que.pop();
        _notFull.notify_one();
    }
    return result;
}

void TaskQueue::wakeUpAllThread(){
    _is_stop = true;
    _notEmpty.notify_all();
}