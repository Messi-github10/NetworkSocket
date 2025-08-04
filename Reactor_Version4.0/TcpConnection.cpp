#include "TcpConnection.hpp"
#include "EventLoop.hpp"
#include <string.h>
#include <iostream>
#include <sstream>
using std::cout;
using std::endl;
using std::ostringstream;

TcpConnection::TcpConnection(int fd, EventLoop *loop)
    : _sock(fd), _socketIO(fd), _localAddr(getLocalAddr(fd)), _peerAddr(getPeerAddr(fd)), _isShutDownWrite(false), _loop(loop), _onConnection(nullptr), _onMessage(nullptr), _onClose(nullptr)
{
}

TcpConnection::~TcpConnection()
{
    if (_isShutDownWrite)
    {
        _sock.shutDownWrite();
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

void TcpConnection::setEventLoop(EventLoop *loop)
{
    _loop = loop;
}

EventLoop *TcpConnection::getEventLoop() const
{
    return _loop;
}

void TcpConnection::broadcast(const string &message, const TcpConnectionPtr &excluede)
{
    if (!_loop)
    {
        cout << "EventLoop pointer is null." << endl;
        return;
    }
    const auto &connections = _loop->getConnections();
    for (auto &it : connections)
    {
        if (!excluede || it.second != excluede)
        {
            it.second->send(message);
        }
    }
}

string TcpConnection::toString() const
{
    ostringstream oss;
    oss << "Tcp Socket:" << _sock.get_fd() << "["
        << _localAddr.get_ip() << ":" << _localAddr.get_port()
        << " -> " << _peerAddr.get_ip() << ":" << _peerAddr.get_port() << "].";
    return oss.str();
}

void TcpConnection::shutdown()
{
    if (!_isShutDownWrite)
    {
        _isShutDownWrite = true;
        _sock.shutDownWrite();
    }
}

void TcpConnection::setAllCallBacks(const TcpConnectionCallBack &onConnection,
                                    const TcpConnectionCallBack &onMessage,
                                    const TcpConnectionCallBack &onClose)
{
    _onConnection = onConnection;
    _onMessage = onMessage;
    _onClose = onClose;
}

void TcpConnection::handleConnectionCallBack()
{
    if (_onConnection)
    {
        _onConnection(shared_from_this());
    }
}

void TcpConnection::handleMessageCallBack()
{
    if (_onMessage)
    {
        _onMessage(shared_from_this());
    }
}

void TcpConnection::handleCloseCallBack()
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

// 将数据发送操作转移到 EventLoop 的事件循环中执行
void TcpConnection::sendInLoop(const string &message)
{
    if (_loop)
    {
        _loop->run_in_loop(std::bind(&TcpConnection::send, this, message));
    }
}


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