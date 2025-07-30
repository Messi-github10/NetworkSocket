#include "InetAddress.hpp"
#include <string.h>
#include <arpa/inet.h>

InetAddress::InetAddress(unsigned short port, const string &ip){
    memset(&_addr, 0, sizeof(_addr));
    _addr.sin_family = AF_INET;
    _addr.sin_addr.s_addr = inet_addr(ip.c_str());
    _addr.sin_port = htons(port);
}

InetAddress::InetAddress(struct sockaddr_in &addr){
    memset(&_addr, 0, sizeof(_addr));
    _addr = addr;
}

InetAddress::~InetAddress() = default;

string InetAddress::ip() const{
    return string(inet_ntoa(_addr.sin_addr));
}

unsigned short InetAddress::port() const{
    return ntohs(_addr.sin_port);
}

const struct sockaddr_in *InetAddress::getAddrPtr() const{
    return &_addr;
}