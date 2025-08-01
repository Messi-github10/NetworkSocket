#include "InetAddress.hpp"

InetAddress::InetAddress(unsigned short port, const string &ip){
    memset(&_addr, 0, sizeof(_addr));
    _addr.sin_family = AF_INET;
    _addr.sin_addr.s_addr = inet_addr(ip.c_str());
    _addr.sin_port = htons(port);
}

InetAddress::InetAddress(struct sockaddr_in &addr)
:_addr(addr)
{}

string InetAddress::get_ip() const{
    return string(inet_ntoa(_addr.sin_addr));
}

unsigned short InetAddress::get_port() const{
    return ntohs(_addr.sin_port);
}

const struct sockaddr_in *InetAddress::getAddrPtr() const{
    return &_addr;
}
