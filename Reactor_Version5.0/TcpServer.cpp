#include "TcpServer.hpp"

TcpServer::TcpServer(unsigned short port, const string ip)
:_acceptor(port, ip)
,_loop(_acceptor)
{}

void TcpServer::start(){
    _acceptor.ready();
    _loop.loop();
}

void TcpServer::stop(){
    _loop.unloop();
}

void TcpServer::setAllCallBacks(TcpConnectionCallBack &&onConnection,
    TcpConnectionCallBack && onMessage,
    TcpConnectionCallBack && onClose){
    _loop.setAllCallback(std::move(onConnection),
    std::move(onMessage), 
    std::move(onClose));
}
