#ifndef TCPSERVER_HPP
#define TCPSERVER_HPP

#include "Acceptor.hpp"
#include "EventLoop.hpp"
#include "TcpConnection.hpp"

class TcpServer{
public:
    TcpServer(unsigned short port, const string ip = "0.0.0.0");
    ~TcpServer() = default;
    void start();
    void stop();
    void setAllCallBacks(TcpConnectionCallBack &&, TcpConnectionCallBack &&, TcpConnectionCallBack &&);

private:
    Acceptor _acceptor;
    EventLoop _loop;
};

#endif