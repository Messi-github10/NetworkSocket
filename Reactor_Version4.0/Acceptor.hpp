#ifndef ACCEPTOR_HPP
#define ACCEPTOR_HPP

#include "InetAddress.hpp"
#include "Socket.hpp"

class Acceptor{
public:
    Acceptor(unsigned short port, const string &ip = "0.0.0.0");
    ~Acceptor() = default;
    void ready();
    int accept();
    int get_listen_fd() const;

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