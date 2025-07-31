#ifndef ACCEPTOR_HPP
#define ACCEPTOR_HPP

#include "InetAddress.hpp"
#include "Socket.hpp"
#include <string>
using std::string;

/*
Acceptor类：连接器类。
将所有基本操作全部封装到该类中。
包括：地址复用、端口复用、bind、listen、accept函数。
*/

class Acceptor{
public:
    Acceptor(unsigned short port);
    Acceptor(unsigned short port, const string &ip = "0.0.0.0");
    ~Acceptor() = default;
    void ready();
    int accept();
    int fd() const;

private:
    void setReuseAddr(bool);
    void setReusePort(bool);
    void bind();
    void listen();

private:
    InetAddress _addr;
    Socket _listen_sock;
};

#endif