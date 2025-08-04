#ifndef INETADDRESS_HPP
#define INETADDRESS_HPP

#include <netinet/in.h>
#include <string>
using std::string;

class InetAddress{
public:
    InetAddress(unsigned short port, const string &ip = "0.0.0.0");
    InetAddress(struct sockaddr_in &);
    ~InetAddress() = default;
    string get_ip() const;
    unsigned short get_port() const;
    const struct sockaddr_in *getAddrPtr() const;

private:
    struct sockaddr_in _addr;
};

#endif