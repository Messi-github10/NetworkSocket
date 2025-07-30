#ifndef INETADDRESS_HPP
#define INETADDRESS_HPP

#include <sys/socket.h>
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
    InetAddress(unsigned short port, const string &ip = "0.0.0.0");
    InetAddress(struct sockaddr_in &addr);
    ~InetAddress();
    string ip() const;
    unsigned short port() const;
    const struct sockaddr_in *getAddrPtr() const;

private:
    struct sockaddr_in _addr;
};

#endif