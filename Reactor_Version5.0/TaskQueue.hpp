#ifndef TASKQUEUE_HPP
#define TASKQUEUE_HPP

#include "Task.hpp"
#include "MutexLock.hpp"
#include "Condition_va.hpp"
#include <queue>
using namespace std;

// 任务队列类
using ElemType = Task;
class TaskQueue
{
public:

    TaskQueue(int queSize)
    :_que()
    ,_queSize(queSize)
    ,_mutex()
    ,_notFull(_mutex)
    ,_notEmpty(_mutex)
    ,_is_stop(false)
    {}

    ~TaskQueue() = default;

    bool full() const;  // 是否满
    bool empty() const; // 是否空
    void push(ElemType);    // 满则阻塞
    ElemType pop();         // 空则阻塞
    void wakeUpAllThread(); // 强制唤醒所有线程执行优雅退出

private:
    queue<ElemType> _que;
    int _queSize;
    MutexLock _mutex;
    Condition_va _notFull;
    Condition_va _notEmpty;
    bool _is_stop;  // 任务队列退出标志
};

#endif