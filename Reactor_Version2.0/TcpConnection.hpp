#ifndef TCPCONNECTION_HPP
#define TCPCONNECTION_HPP

#include "Noncopyable.hpp"
#include "Socket.hpp"
#include "InetAddress.hpp"
#include "SocketIO.hpp"
#include <memory>
#include <string>
#include <functional>
using std::shared_ptr;
using std::string;

class TcpConnection;
using TcpConnectionPtr = shared_ptr<TcpConnection>;
using TcpConnectionCallback = std::function<void(TcpConnectionPtr)>;

class TcpConnection : public Noncopyable{
public:
    TcpConnection(int);
    ~TcpConnection();
    string receive();
    void send(const string &);
    string toString() const;    // 获取五元组信息
    void shutdown();
    void setAllCallbacks(const TcpConnectionCallback &, const TcpConnectionCallback &, const TcpConnectionCallback &);
    void handleConnectionCallback();
    void handleMessageCallback();
    void handleCloseCallback();
    bool isClosed() const;

private:
    InetAddress getLocalAddr(int);
    InetAddress getPeerAddr(int);

private:
    Socket _sock;
    SocketIO _socketIO;
    InetAddress _localAddr;
    InetAddress _peerAddr;
    bool _isShutdownWrite;

    TcpConnectionCallback _onConnection;
    TcpConnectionCallback _onMessage;
    TcpConnectionCallback _onClose;
};

#endif