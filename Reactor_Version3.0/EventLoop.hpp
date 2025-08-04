#ifndef EVENTLOOP_HPP
#define EVENTLOOP_HPP

#include "TcpConnection.hpp"
#include "Acceptor.hpp"
#include <sys/epoll.h>
#include <vector>
#include <unordered_map>
using std::unordered_map;
using std::vector;

class EventLoop{
public:
    EventLoop(Acceptor &);
    ~EventLoop() = default;
    void loop();
    void unloop();
    void setAllCallback(TcpConnectionCallBack &&, TcpConnectionCallBack &&, TcpConnectionCallBack &&);
    const unordered_map<int, TcpConnectionPtr> &getConnections() const;

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
    unordered_map<int, TcpConnectionPtr> _conns;

    TcpConnectionCallBack _onConnection;
    TcpConnectionCallBack _onMessage;
    TcpConnectionCallBack _onClose;
};

#endif