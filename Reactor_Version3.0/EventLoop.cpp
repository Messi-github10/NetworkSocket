#include "EventLoop.hpp"
#include <string.h>

#define MAX_EVENTS 100
#define TIMEOUT 10000

EventLoop::EventLoop(Acceptor &acceptor)
:_epoll_fd(create_epoll_fd())
,_isLooping(false)
,_acceptor(acceptor)
,_ready_events(MAX_EVENTS)
,_conns()
,_onConnection(nullptr)
,_onMessage(nullptr)
,_onClose(nullptr)
{
    add_epoll_read_event(_acceptor.get_listen_fd());
}

void EventLoop::loop(){
    _isLooping = true;
    while(_isLooping){
        wait_epoll_fd();
    }
}

void EventLoop::unloop(){
    _isLooping = false;
}

void EventLoop::setAllCallback(TcpConnectionCallBack && onConnection,
    TcpConnectionCallBack && onMessage,
    TcpConnectionCallBack && onClose){
    _onConnection = onConnection;
    _onMessage = onMessage;
    _onClose = onClose;
}

const unordered_map<int, TcpConnectionPtr> &EventLoop::getConnections() const{
    return _conns;
}

int EventLoop::create_epoll_fd(){
    int epoll_fd = epoll_create1(0);
    if(epoll_fd < 0){
        perror("epoll_create1");
    }
    return epoll_fd;
}

void EventLoop::add_epoll_read_event(int fd){
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLIN;
    ev.data.fd = fd;
    int ret = epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &ev);
    if(ret < 0){
        perror("epoll_ctl");
    }
}

void EventLoop::delete_epoll_read_event(int fd){
    int ret = epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
    if(ret < 0){
        perror("epoll_ctl");
    }
}

void EventLoop::wait_epoll_fd(){
    int ready_num = epoll_wait(_epoll_fd, _ready_events.data(), MAX_EVENTS, TIMEOUT);
    if(ready_num == -1 && errno == EINTR){
        return;
    }else if(ready_num == -1){
        perror("epoll_wait");
        return;
    }else if(ready_num == 0){
        printf("epoll_timeout.\n");
    }else{
        for (int i = 0; i < ready_num; i++){
            int cur_fd = _ready_events[i].data.fd;
            if (cur_fd == _acceptor.get_listen_fd()){
                handle_new_connection();
            }else{
                handle_message(cur_fd);
            }
        }
    }
}

void EventLoop::handle_new_connection(){
    int peer_fd = _acceptor.accept();
    add_epoll_read_event(peer_fd);
    
    TcpConnectionPtr tcp_connection_ptr(new TcpConnection(peer_fd));
    tcp_connection_ptr->setEventLoop(this);
    _conns.insert(std::make_pair(peer_fd, tcp_connection_ptr));

    tcp_connection_ptr->setAllCallBacks(_onConnection, _onMessage, _onClose);
    tcp_connection_ptr->handleConnectionCallBack();
}

void EventLoop::handle_message(int fd){
    auto tcp_connection_iterator = _conns.find(fd);
    if(tcp_connection_iterator != _conns.end()){
        bool isClosed = tcp_connection_iterator->second->isClosed();
        if(isClosed){
            tcp_connection_iterator->second->hanleCloseCallBack();
            delete_epoll_read_event(fd);
            _conns.erase(fd);
        }else{
            tcp_connection_iterator->second->handleMessageCallBack();
        }
    }
}