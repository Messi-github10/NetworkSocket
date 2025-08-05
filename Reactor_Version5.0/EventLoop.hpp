#ifndef EVENTLOOP_HPP
#define EVENTLOOP_HPP

#include "MutexLock.hpp"
#include "TcpConnection.hpp"
#include "Acceptor.hpp"
#include <sys/epoll.h>
#include <vector>
#include <unordered_map>
#include <functional>
#include <sys/eventfd.h>
using std::unordered_map;
using std::vector;

// EventLoop类：事件调度中枢

using Functor = std::function<void()>;  // 统一封装任意无参且无返回值的可调用对象
class EventLoop{
public:
    // 创建epoll_fd监听新事件的到来、创建event_fd监听子线程通知数据处理完成后进行数据发送操作
    EventLoop(Acceptor &);
    ~EventLoop();   // 关闭epoll和event的文件描述符
    void loop();    // 开启事件循环 -> 开始 wait_epoll 监听
    void unloop();  // 关闭事件循环
    void setAllCallback(TcpConnectionCallBack &&, TcpConnectionCallBack &&, TcpConnectionCallBack &&);  // 设置回调函数
    const unordered_map<int, TcpConnectionPtr> &getConnections() const; // 安全地获取当前所有活跃的 TCP 连接列表
    void handleReadEvent();  // 处理 eventfd 的可读事件
    void wakeup();  // 唤醒因为epoll_wait阻塞的 I/O 线程
    void doPendingFunctors();   // 执行任务，将数据发给客户端
    void run_in_loop(Functor &&);   // 将一个任务（Functor）​安全地提交到 EventLoop 所在的 I/O 线程中执行


private:
    int create_epoll_fd();  // 创建epoll的文件描述符
    int create_event_fd();  // 创建事件通知描述符
    void add_epoll_read_event(int); // 添加读事件监听到epoll红黑树当中
    void delete_epoll_read_event(int);  // 删除读事件监听到epoll红黑树中
    void wait_epoll_fd();   // 循环监听是否有读事件到来
    void handle_new_connection();   // 处理新连接到来的函数
    void handle_message(int);   // 处理消息到来的函数

private:
    int _epoll_fd;
    int _event_fd;
    bool _isLooping;
    Acceptor &_acceptor;

    vector<struct epoll_event> _ready_events;   // 就绪事件列表
    unordered_map<int, TcpConnectionPtr> _conns;    // 当前活跃的 TCP 连接列表

    // 列表中存储待执行的跨线程任务
    // 防止一个线程在操作该列表的时候，其他线程又对该列表进行插入操作，导致 vector 中的数据不一致
    // 若不互斥访问，可能会导致数组越界、迭代器失效等问题
    vector<Functor> _pendingFunctors;
    MutexLock _mutex;   // 互斥锁保证互斥访问跨线程任务列表

    TcpConnectionCallBack _onConnection;    // 新连接回调函数
    TcpConnectionCallBack _onMessage;       // 消息到来回调函数
    TcpConnectionCallBack _onClose;         // 关闭连接回调函数
};

#endif