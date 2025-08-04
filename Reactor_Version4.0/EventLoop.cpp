#include "EventLoop.hpp"
#include "SmartMutexLock.hpp"
#include <string.h>
#include <unistd.h>

#define MAX_EVENTS 100
#define TIMEOUT 10000

EventLoop::EventLoop(Acceptor &acceptor)
    : _epoll_fd(create_epoll_fd()),
      _event_fd(create_event_fd()),
      _isLooping(false), 
      _acceptor(acceptor), 
      _ready_events(MAX_EVENTS), 
      _conns(), 
      _pendingFunctors(), 
      _mutex(), 
      _onConnection(nullptr), 
      _onMessage(nullptr), 
      _onClose(nullptr)
{
    add_epoll_read_event(_acceptor.get_listen_fd());
    add_epoll_read_event(_event_fd);
}

EventLoop::~EventLoop()
{
    close(_epoll_fd);
    close(_event_fd);
}

void EventLoop::loop()
{
    _isLooping = true;
    while (_isLooping)
    {
        wait_epoll_fd();
    }
}

void EventLoop::unloop()
{
    _isLooping = false;
}

void EventLoop::setAllCallback(TcpConnectionCallBack &&onConnection,
                               TcpConnectionCallBack &&onMessage,
                               TcpConnectionCallBack &&onClose)
{
    _onConnection = onConnection;
    _onMessage = onMessage;
    _onClose = onClose;
}

const unordered_map<int, TcpConnectionPtr> &EventLoop::getConnections() const
{
    return _conns;
}

// 创建事件通知描述符
int EventLoop::create_event_fd()
{
    int event_fd = eventfd(0, 0);
    if (event_fd < 0)
    {
        perror("eventfd");
    }
    return event_fd;
}

// 处理 eventfd 的可读事件
void EventLoop::handleReadEvent()
{
    // 它是一个 ​8 字节（uint64_t）的缓冲区
    // 用于接收 read() 从 eventfd 读取的计数器值
    uint64_t dummy = 0; // 但是此处dummy无实际意义
    int ret = read(_event_fd, &dummy, sizeof(dummy));
    if (ret != sizeof(dummy))
    { // 保证完整读取八个字节的数据
        perror("read");
    }
}

// 唤醒因为epoll_wait阻塞的 I/O 线程
void EventLoop::wakeup()
{
    // 只要往内核计数器上 + 1，就能改变其值，实现通知操作
    // 内核计数器 += value
    uint64_t dummy = 1;
    int ret = write(_event_fd, &dummy, sizeof(dummy));
    if (ret != sizeof(dummy))
    {
        perror("write");
    }
}

// 执行任务，将数据发给客户端
void EventLoop::doPendingFunctors()
{
    printf("Start to doPendingFunctors...\n");
    // _pendingFunctors是临界资源，应该尽快将其释放，交给计算线程
    vector<Functor> temp_vec;

    // 使用 swap 的目的是高效且线程安全地转移待执行任务
    // 如果直接遍历 _pendingFunctors，在执行任务期间，新任务会追加到 _pendingFunctors 末尾
    // 无限循环​：新任务不断加入，当前循环无法结束。
    // 延迟响应​：新任务必须等待当前所有任务执行完。
    {
        SmartMutexLock smartMutex(_mutex);
        temp_vec.swap(_pendingFunctors);
    }
    for (auto &func : temp_vec)
    {
        func();
    }
}

// 将一个任务（Functor）​安全地提交到 EventLoop 所在的 I/O 线程中执行
void EventLoop::run_in_loop(Functor &&FunctionCallBack)
{
    {
        SmartMutexLock smartMutex(_mutex);
        _pendingFunctors.push_back(FunctionCallBack);
    }
    wakeup(); // 通知IO线程
}

int EventLoop::create_epoll_fd()
{
    int epoll_fd = epoll_create1(0);
    if (epoll_fd < 0)
    {
        perror("epoll_create1");
    }
    return epoll_fd;
}

void EventLoop::add_epoll_read_event(int fd)
{
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLIN;
    ev.data.fd = fd;
    int ret = epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &ev);
    if (ret < 0)
    {
        perror("epoll_ctl");
    }
}

void EventLoop::delete_epoll_read_event(int fd)
{
    int ret = epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
    if (ret < 0)
    {
        perror("epoll_ctl");
    }
}

void EventLoop::wait_epoll_fd()
{
    int ready_num = epoll_wait(_epoll_fd, _ready_events.data(), MAX_EVENTS, TIMEOUT);
    if (ready_num == -1 && errno == EINTR)
    {
        return;
    }
    else if (ready_num == -1)
    {
        perror("epoll_wait");
        return;
    }
    else if (ready_num == 0)
    {
        printf("epoll_timeout.\n");
    }
    else
    {
        for (int i = 0; i < ready_num; i++)
        {
            int cur_fd = _ready_events[i].data.fd;
            if (cur_fd == _acceptor.get_listen_fd())
            {
                handle_new_connection();
            }else if(cur_fd == _event_fd){
                handleReadEvent();
                doPendingFunctors();
            }
            else
            {
                handle_message(cur_fd);
            }
        }
    }
}

void EventLoop::handle_new_connection()
{
    int peer_fd = _acceptor.accept();
    add_epoll_read_event(peer_fd);

    TcpConnectionPtr tcp_connection_ptr(new TcpConnection(peer_fd, this));
    tcp_connection_ptr->setEventLoop(this);
    _conns.insert(std::make_pair(peer_fd, tcp_connection_ptr));

    tcp_connection_ptr->setAllCallBacks(_onConnection, _onMessage, _onClose);
    tcp_connection_ptr->handleConnectionCallBack();
}

void EventLoop::handle_message(int fd)
{
    auto tcp_connection_iterator = _conns.find(fd);
    if (tcp_connection_iterator != _conns.end())
    {
        bool isClosed = tcp_connection_iterator->second->isClosed();
        if (isClosed)
        {
            tcp_connection_iterator->second->handleCloseCallBack();
            delete_epoll_read_event(fd);
            _conns.erase(fd);
        }
        else
        {
            tcp_connection_iterator->second->handleMessageCallBack();
        }
    }
}