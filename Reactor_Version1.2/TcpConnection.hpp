#ifndef TCPCONNECTION_HPP
#define TCPCONNECTION_HPP

#include "Socket.hpp"
#include "InetAddress.hpp"
#include "SocketIO.hpp"
#include <string>
using std::string;

#include "Noncopyable.hpp"

/*
TcpConnection类：
该类的创建是由Acceptor类的对象调用accept函数得来的，也就是表明三次握手建立成功之后的结果。
可以通过该连接的对象进行收发数据，发送数据的时候封装函数send，接受数据的时候封装函数receive。
*/

class TcpConnection : public Noncopyable{
public:
    TcpConnection(int);
    ~TcpConnection();
    void send(const string &);
    string receive();
    void sendWithPrefix(const string &);
    string receiveWithPrefix();
    string toString() const;
    void shutdown();

private:
    InetAddress getLocalAddress() const;
    InetAddress getPeerAddress() const;

private:
    Socket _sock;
    InetAddress _local_addr;
    InetAddress _peer_addr;
    SocketIO _socketIO;
};

#endif