#ifndef EVENTLOOP_HPP
#define EVENTLOOP_HPP

#include "TcpConnection.hpp"
#include "Acceptor.hpp"
#include <vector>
#include <map>
using std::map;
using std::vector;

class EventLoop{
public:
    EventLoop(Acceptor &);
    ~EventLoop() = delete;
    void loop();
    void setAllCallback(TcpConnectionCallback &&, TcpConnectionCallback &&, TcpConnectionCallback &&);
    
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
    map<int, TcpConnection> _conn;  // 记录了当前事件循环中所有活跃的 TCP 连接

    TcpConnectionCallback _onConnection;
    TcpConnectionCallback _onMessage;
    TcpConnectionCallback _onClose;
};

#endif