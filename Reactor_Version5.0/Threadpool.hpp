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
    void startThreadPool(); // 开启线程池
    void stopThreadPool();  // 关闭线程池
    void addTask(Task &&);  // 移动语义：只传右值，提升效率
    void doTask();  // 执行任务

private:
    vector<unique_ptr<MyThread>> _threads;
    int _threadNum;
    TaskQueue _taskque;
    int _queSize;
    bool _isExit;
};

#endif