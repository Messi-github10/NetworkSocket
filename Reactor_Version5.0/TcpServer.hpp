#ifndef TCPSERVER_HPP
#define TCPSERVER_HPP

#include "Acceptor.hpp"
#include "EventLoop.hpp"
#include "TcpConnection.hpp"

// TcpServer集成了Acceptor和EventLoop，提供了更友好的接口给外部
class TcpServer{
public:
    TcpServer(unsigned short port, const string ip = "0.0.0.0");
    ~TcpServer() = default;
    void start();   // 开启服务器
    void stop();    // 关闭服务器
    void setAllCallBacks(TcpConnectionCallBack &&, TcpConnectionCallBack &&, TcpConnectionCallBack &&); // 设置回调函数

private:
    Acceptor _acceptor;
    EventLoop _loop;
};

#endif