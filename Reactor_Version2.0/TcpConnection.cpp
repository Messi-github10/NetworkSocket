#include "TcpConnection.hpp"
#include "EventLoop.hpp"
#include <sstream>
#include <iostream>
using std::cout;
using std::endl;
using std::ostringstream;

TcpConnection::TcpConnection(int fd)
    : _sock(fd), _socketIO(fd), _localAddr(getLocalAddr(fd)), _peerAddr(getPeerAddr(fd)), _isShutdownWrite(false), _onConnection(nullptr), _onMessage(nullptr), _onClose(nullptr)
{
}

TcpConnection::~TcpConnection()
{
    if (!_isShutdownWrite)
    {
        shutdown();
    }
}

string TcpConnection::receive()
{
    char buffer[65536] = {0};
    _socketIO.readline(buffer, sizeof(buffer));
    return string(buffer);
}

void TcpConnection::send(const string &message)
{
    _socketIO.sendn(message.c_str(), message.size());
}

// 获取五元组信息
string TcpConnection::toString() const
{
    ostringstream oss;
    oss << "Tcp Socket:" << _sock.get_fd() << " ["
        << _localAddr.get_ip() << ":" << _localAddr.get_port()
        << " -> " << _peerAddr.get_ip() << ":" << _peerAddr.get_port();
    return oss.str();
}

void TcpConnection::shutdown()
{
    if (!_isShutdownWrite)
    {
        _isShutdownWrite = true;
        _sock.shutDownWrite();
    }
}

void TcpConnection::setAllCallbacks(const TcpConnectionCallback &onConnection,
                                    const TcpConnectionCallback &onMessage,
                                    const TcpConnectionCallback &onClose)
{
    _onConnection = onConnection;
    _onMessage = onMessage;
    _onClose = onClose;
}

void TcpConnection::handleConnectionCallback()
{
    if (_onConnection)
    {
        _onConnection(shared_from_this());
    }
}

void TcpConnection::handleMessageCallback()
{
    if (_onMessage)
    {
        _onMessage(shared_from_this());
    }
}

void TcpConnection::handleCloseCallback()
{
    if (_onClose)
    {
        _onClose(shared_from_this());
    }
}

bool TcpConnection::isClosed() const
{
    char buffer[20] = {0};
    return (_socketIO.recvPeek(buffer, sizeof(buffer)) == 0 ? true : false);
}

// accept_fd
InetAddress TcpConnection::getLocalAddr(int fd)
{
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    socklen_t len = sizeof(struct sockaddr);
    int ret = getsockname(fd, (struct sockaddr *)&addr, &len);
    if (ret < 0)
    {
        perror("getsockname");
    }
    return InetAddress(addr);
}

// accept_fd
InetAddress TcpConnection::getPeerAddr(int fd)
{
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    socklen_t len = sizeof(struct sockaddr);
    int ret = getpeername(fd, (struct sockaddr *)&addr, &len);
    if (ret < 0)
    {
        perror("getpeername");
    }
    return InetAddress(addr);
}

void TcpConnection::setEventLoop(EventLoop *loop){
    _loop = loop;
}

EventLoop *TcpConnection::getEventLoop() const{
    return _loop;
}

void TcpConnection::broadcast(const string &message, const TcpConnectionPtr &excluede){
    if(!_loop){
        cout << "EventLoop pointer is null." << endl;
        return;
    }

    const auto &connections = _loop->getConnections();
    for(auto & it : connections){
        if(!excluede || it.second != excluede){
            // 不排除任何连接 或者 排除指定连接
            it.second->send(message);
        }
    }
}