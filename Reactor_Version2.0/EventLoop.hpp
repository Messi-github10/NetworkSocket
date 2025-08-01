#ifndef EVENTLOOP_HPP
#define EVENTLOOP_HPP

#include "TcpConnection.hpp"
#include "Acceptor.hpp"
#include <sys/epoll.h>
#include <vector>
#include <map>
using std::map;
using std::vector;

// 单线程下EventLoop对象全局唯一
class EventLoop{
public:
    EventLoop(Acceptor &);
    ~EventLoop() = default;
    void loop();
    void unloop();  // 退出循环，要与loop函数运行在不同的线程中
    void setAllCallback(TcpConnectionCallback &&, TcpConnectionCallback &&, TcpConnectionCallback &&);
    const map<int, TcpConnectionPtr> &getConnections() const;
    
private:
    int create_epoll_fd();
    void add_epoll_read_event(int);
    void delete_epoll_read_event(int);
    void wait_epoll_fd();
    void handle_new_connection();
    void handle_message(int);

private:
    int _epoll_fd;
    bool _isLooping;
    Acceptor &_acceptor;
    vector<struct epoll_event> _ready_events;
    map<int, TcpConnectionPtr> _conns;  // 记录了当前事件循环中所有活跃的 TCP 连接

    TcpConnectionCallback _onConnection;    // 新连接事件处理器
    TcpConnectionCallback _onMessage;       // 信息处理事件处理器
    TcpConnectionCallback _onClose;         // 关闭连接事件处理器
};

#endif