#ifndef TASKQUEUE_HPP
#define TASKQUEUE_HPP

#include "Task.hpp"
#include "MutexLock.hpp"
#include "Condition_va.hpp"
#include <queue>
using namespace std;

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

    bool full() const;
    bool empty() const;
    void push(ElemType);
    ElemType pop();
    void wakeUpAllThread();

private:
    queue<ElemType> _que;
    int _queSize;
    MutexLock _mutex;
    Condition_va _notFull;
    Condition_va _notEmpty;
    bool _is_stop;
};

#endif