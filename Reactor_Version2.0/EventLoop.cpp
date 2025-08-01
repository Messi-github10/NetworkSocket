#include "EventLoop.hpp"
#include <sys/epoll.h>

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
    // 加入监听新连接事件
    add_epoll_read_event(_acceptor.get_listen_fd());
}

void EventLoop::loop(){
    _isLooping = true;
    while(_isLooping){
        // 循环等待监听事件
        wait_epoll_fd();
    }
}

void EventLoop::unloop(){
    // TODO
}

void EventLoop::setAllCallback(TcpConnectionCallback && onConnection
    , TcpConnectionCallback && onMessage
    , TcpConnectionCallback && onClose){
        // 避免拷贝的开销，希望走移动赋值运算函数
    _onConnection = std::move(onConnection);
    _onMessage = std::move(onMessage);
    _onClose = std::move(onClose);
}

int EventLoop::create_epoll_fd(){
    int epoll_fd = epoll_create1(0);
    if(epoll_fd < 0){
        perror("epoll_create1");
    }
    return epoll_fd;
}

// 形参中的fd是listen_fd
void EventLoop::add_epoll_read_event(int fd){
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLIN;    // 读事件
    ev.data.fd = fd;
    int ret = epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &ev);
    if(ret < 0){
        perror("epoll_ctl");
    }
}

// 形参中的fd是listen_fd
void EventLoop::delete_epoll_read_event(int fd){
    int ret = epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
    if(ret < 0){
        perror("epoll_ctl");
    }
}

void EventLoop::wait_epoll_fd(){
    // 事件循环
    int ready_num = epoll_wait(_epoll_fd, _ready_events.data(), _ready_events.size(), TIMEOUT);
    if(ready_num == -1 && errno == EINTR){
        // 软中断错误
        return;
    }else if(ready_num == -1){
        perror("epoll_wait");
        return;
    }else if(ready_num == 0){
        // 超时
        printf("epoll_timeout.\n");
    }else{
        // 正常情况
        for (int i = 0; i < ready_num; i++){
            int cur_fd = _ready_events[i].data.fd;
            if(cur_fd == _acceptor.get_listen_fd()){
                // 新连接的处理
                handle_new_connection();
            }else{
                // 已经建立好的连接
                // 数据处理
                handle_message(cur_fd);
            }
        }
    }
}

// 处理新连接
void EventLoop::handle_new_connection(){
    // epoll对accept_fd读事件的监听
    int client_fd = _acceptor.accept();

    // client_fd添加到epoll的红黑树中
    add_epoll_read_event(client_fd);

    TcpConnectionPtr tcp_connection_ptr(new TcpConnection(client_fd));
    tcp_connection_ptr->setEventLoop(this);
    _conns.insert(std::make_pair(client_fd, tcp_connection_ptr));

    // 注册回调函数
    tcp_connection_ptr->setAllCallbacks(_onConnection, _onMessage, _onClose);

    // 调用建立连接的回调函数
    tcp_connection_ptr->handleConnectionCallback();
}

// 数据处理
void EventLoop::handle_message(int fd){
    // 事件分发
    // accept_fd
    auto tcp_connection_iterator = _conns.find(fd);
    if(tcp_connection_iterator != _conns.end()){
        // 先判断连接是否断开
        bool isClosed = tcp_connection_iterator->second->isClosed();
        if(isClosed){
            // 断开连接
            tcp_connection_iterator->second->handleCloseCallback();
            // 从红黑树上删除
            delete_epoll_read_event(fd);
            _conns.erase(fd);   // 从map中删除掉记录
        }else{
            // 连接未断开，处理消息
            tcp_connection_iterator->second->handleMessageCallback();
        }
    }
}

const map<int, TcpConnectionPtr> & EventLoop::getConnections() const{
    return _conns;
}
