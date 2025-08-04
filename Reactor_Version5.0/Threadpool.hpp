#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP

#include "MyThread.hpp"
#include "TaskQueue.hpp"
#include "Task.hpp"
#include <vector>
#include <memory>
using namespace std;

class Threadpool{
public:
    Threadpool(int, int);
    ~Threadpool() = default;
    void startThreadPool();
    void stopThreadPool();
    void addTask(Task &&);  // 移动语义：只传右值，提升效率
    void doTask();

private:
    vector<unique_ptr<MyThread>> _threads;
    int _threadNum;
    TaskQueue _taskque;
    int _queSize;
    bool _isExit;
};

#endif