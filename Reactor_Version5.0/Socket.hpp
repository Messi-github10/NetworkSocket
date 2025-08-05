#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "Noncopyable.hpp"

/*
Socket类：
将所有与套接字相关的操作全部封装到该类中。
包括：套接字（文件描述符）的创建、文件描述符的关闭、获取文件描述符。
*/

class Socket : public Noncopyable{
public:
    Socket();       // 创建文件描述符
    Socket(int);    // 接收并保存一个现有的文件描述符（fd）
    ~Socket();      // 关闭文件描述符
    int get_fd() const; // 获取文件描述符
    void shutDownWrite();   // 关闭写端

private:
    int _fd;
};

#endif