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
    for (int i = 0; i < _threadNum; i++){
        unique_ptr<MyThread> pTh(new MyThread(bind(&Threadpool::doTask, this)));    // 创建一个由智能指针管理的线程对象
        _threads.push_back(std::move(pTh));     //  ​将线程对象的独占所有权转移（move）到线程池的线程列表 _threads，由 _threads 管理
    }

    for(auto & thread : _threads){
        thread->start();    // 让每个线程运行起来
    }
}

void Threadpool::stopThreadPool(){
    while(!_taskque.empty()){   // 等待线程池中的所有任务结束后再退出线程池
        sleep(1);
    }

    _isExit = true;

    _taskque.wakeUpAllThread(); //  ​强制唤醒所有可能阻塞在任务队列上的工作线程，确保它们能检测到线程池关闭标志并安全退出

    for(auto  & thread : _threads){
        // 阻塞主线程（即开启线程池的线程）
        thread->join(); // 等待线程池中的每一个子线程完成
    }
}

void Threadpool::addTask(Task &&task){
    if(task){
        _taskque.push(std::move(task)); // 将任务添加到任务队列中
    }
}

void Threadpool::doTask(){
    while(!_isExit){
        Task task = _taskque.pop(); // 将任务从任务队列中弹出
        if(task){
            task(); // 执行任务
        }
    }
}