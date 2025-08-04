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

using Functor = std::function<void()>;

class EventLoop{
public:
    EventLoop(Acceptor &);
    ~EventLoop();
    void loop();
    void unloop();
    void setAllCallback(TcpConnectionCallBack &&, TcpConnectionCallBack &&, TcpConnectionCallBack &&);
    const unordered_map<int, TcpConnectionPtr> &getConnections() const;
    int create_event_fd();  // 创建事件通知描述符
    void handleReadEvent();  // 处理 eventfd 的可读事件
    void wakeup();  // 唤醒因为epoll_wait阻塞的 I/O 线程
    void doPendingFunctors();   // 执行任务，将数据发给客户端
    void run_in_loop(Functor &&);   // 将一个任务（Functor）​安全地提交到 EventLoop 所在的 I/O 线程中执行


private:
    int create_epoll_fd();
    void add_epoll_read_event(int);
    void delete_epoll_read_event(int);
    void wait_epoll_fd();
    void handle_new_connection();
    void handle_message(int);

private:
    int _epoll_fd;
    int _event_fd;
    bool _isLooping;
    Acceptor &_acceptor;

    vector<struct epoll_event> _ready_events;
    unordered_map<int, TcpConnectionPtr> _conns;

    vector<Functor> _pendingFunctors;   // 存储待执行的跨线程任务
    MutexLock _mutex;

    TcpConnectionCallBack _onConnection;
    TcpConnectionCallBack _onMessage;
    TcpConnectionCallBack _onClose;
};

#endif