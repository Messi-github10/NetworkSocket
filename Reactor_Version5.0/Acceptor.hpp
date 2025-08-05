#ifndef ACCEPTOR_HPP
#define ACCEPTOR_HPP

#include "InetAddress.hpp"
#include "Socket.hpp"

/*
Acceptor类：连接器类。
将所有基本操作全部封装到该类中。
包括：地址复用、端口复用、bind、listen、accept函数。
*/

class Acceptor{
public:
    Acceptor(unsigned short port, const string &ip = "0.0.0.0");
    ~Acceptor() = default;
    void ready();   // 完成地址复用、端口复用、绑定端口和IP地址、监听是否有新连接到来
    int accept();   // 接受客户端的连接
    int get_listen_fd() const;  // 获取监听新连接的文件描述符

private:
    void setReuseAddr(bool);    // 地址复用
    void setReusePort(bool);    // 端口复用
    void bind();                // 绑定端口和IP地址
    void listen();              // 监听是否有新连接到来

private:
    InetAddress _addr;
    Socket _listen_sock;
};

#endif