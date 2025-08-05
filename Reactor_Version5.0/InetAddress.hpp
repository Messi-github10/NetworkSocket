#ifndef INETADDRESS_HPP
#define INETADDRESS_HPP

#include <netinet/in.h>
#include <string>
using std::string;

/* 
InetAddress类：
将所有与地址相关的操作都封装到该类中。
包括：通过ip与port构建对象、通过地址对象获取ip、获取端口号。
*/

class InetAddress{
public:
    // 当开发者需要手动指定监听地址或目标地址时，直接传递端口和IP字符串更符合直觉
    InetAddress(unsigned short port, const string &ip = "0.0.0.0");

    // 当从底层网络函数​（如accept/getsockname）获取到sockaddr_in时，直接复用该结构体更高效
    InetAddress(struct sockaddr_in &);
    ~InetAddress() = default;
    string get_ip() const;  // 获取服务器的IP地址
    unsigned short get_port() const;    // 获取服务器的端口
    const struct sockaddr_in *getAddrPtr() const;   // 获取 sockaddr_in 结构体（该结构体记录了IPv4、IP地址、端口信息）

private:
    struct sockaddr_in _addr;
};

#endif